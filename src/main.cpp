#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <PrivateConfig.h>
#include <log.hpp>
#include <WiFiSetup.hpp>
#include <OTASetup.hpp>

#ifdef ALEXA_ENABLE
#include <fauxmoESP.h>
#endif

#define POWER_BUTTON 14
#define RELAY_TRIGGER_OFF 5
#define RELAY_TRIGGER_ON 12

volatile long timestamp;
volatile bool buttonPressed;
volatile bool powerIsOn;
volatile bool wifiConnected;
volatile int watchdogCounter;

static Ticker watchdogTicker;
static ESP8266WebServer server(80);

#ifdef ALEXA_ENABLE
static fauxmoESP fauxmo;
#endif

void restartSystem()
{
  Serial.println();
  LOG0( "*** restarting system ***\n" );
  watchdogTicker.detach();
  delay( 3000 );
  ESP.restart();
  delay( 5000 );
}

void ISRwatchdog()
{
  if ( watchdogCounter++ == 120 )
  {
    Serial.println();
    LOG0( "*** RESET by watchdog ***\n" );
    restartSystem();
  }
}

////////////////////////////////////////////////////////////////////////////////

void openHabSendValue( const char* value )
{
#ifndef OHAB_DISABLE_CALLBACK

  WiFiClient client;

  Serial.println( "Sending to OpenHab");
  Serial.print( "openhab host = ");
  Serial.print( OHAB_HOST );
  Serial.print( ", port = ");
  Serial.println( OHAB_PORT );

  if (!client.connect(OHAB_HOST, OHAB_PORT)) {
    Serial.println("connection failed");
    client.stop();
    return;
  }

  String request = String("GET /CMD?") + OHAB_ITEM + "=" + value + " HTTP/1.1\r\n" +
  #ifdef OHAB_USE_AUTH
               "Authorization: Basic " + OHAB_CREDENTIALS + "\r\n" +
  #endif
               "Host: " + OHAB_HOST + "\r\n" +
               "Connection: close\r\n\r\n";

  //Serial.println( "\n\n--- Request ---\n" + request );

  client.print(request);
  delay(50);

  //Serial.println( "--- Response ---" );
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    //Serial.print(line);
  }

  //Serial.println("\nclosing connection");
  client.stop();

#endif
}

void toggleRelay()
{
  if( powerIsOn )
  {
    digitalWrite( RELAY_TRIGGER_OFF, 0 );
    delay(50);
    digitalWrite( RELAY_TRIGGER_OFF, 1 );
  }
  else
  {
    digitalWrite( RELAY_TRIGGER_ON, 0 );
    delay(50);
    digitalWrite( RELAY_TRIGGER_ON, 1 );
  }
  powerIsOn ^= true;

  LOG1( "toggle power state to: %s\n", (powerIsOn) ? "ON" : "OFF" );
}

void powerButtonPressed()
{
  if (( millis() - timestamp > 100 )) // button debouncing
  {
    buttonPressed = true;
  }
  timestamp = millis();
}

/////////////////////////////////////

void jsonStatus()
{
  String message = "{\"state\":";
  message += ( powerIsOn ) ? "1}\r\n" : "0}\r\n";
  server.send( 200, "application/json", message );
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleRoot()
{
  String message = "<html><body><h1>OpenHAB/Alexa WiFi Socket (" OTA_HOSTNAME ")</h1>";
  message += "<h4>Version " APP_VERSION " by " APP_AUTHOR " </h4>";

  message += "<h4>Current State</h4>power is ";
  message += ( powerIsOn ) ? "on" : "off";
  message += "<h4>Actions</h4><a href='on'>on</a>";
  message += "<br/><a href='off'>off</a>";
  message += "<br/><a href='state'>state</a>";

  message += "<h4>Build Information</h4>";

  message += "Build date = " __DATE__ " " __TIME__ "<br/>";

  #ifdef OHAB_USE_AUTH
    message += "OpenHAB use authentication = true<br/>";
  #else
    message += "OpenHAB use authentication = false<br/>";
  #endif

  #ifdef OHAB_DISABLE_CALLBACK
    message += "OpenHAB disable callback = true<br/>";
  #else
    message += "OpenHAB disable callback  = false<br/>";
  #endif

  message += "</body></html>";
  server.send( 200, "text/html", message );
}

void initWebServer()
{
  server.on("/", handleRoot);
  server.on("/state", []() {
    jsonStatus();
  });
  server.on("/on", []() {
    powerIsOn = false;
    toggleRelay();
    jsonStatus();
  });
  server.on("/off", []() {
    powerIsOn = true;
    toggleRelay();
    jsonStatus();
  });
  server.onNotFound(handleNotFound);
  server.begin();
  LOG0("HTTP server started\n");
  MDNS.addService("http", "tcp", 80);
}

/////////////////////////////////////

void setup() {
  Serial.begin(74880);

  timestamp = millis();
  buttonPressed = false;
  powerIsOn = true;
  wifiConnected = false;

  pinMode( POWER_BUTTON, INPUT_PULLUP);
  pinMode( WIFI_LED, OUTPUT);
  pinMode( RELAY_TRIGGER_OFF, OUTPUT);
  pinMode( RELAY_TRIGGER_ON, OUTPUT);

  digitalWrite( WIFI_LED, 0 );
  digitalWrite( RELAY_TRIGGER_ON, 1 );
  digitalWrite( RELAY_TRIGGER_OFF, 1 );
  toggleRelay();

  ESP.eraseConfig();

  attachInterrupt(digitalPinToInterrupt(POWER_BUTTON), &powerButtonPressed, FALLING);

  delay(2000);

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F("OpenHAB/Alexa WiFi Socket " APP_VERSION " - by " APP_AUTHOR ));
  Serial.println( "Build date: " __DATE__ " " __TIME__ );
  LOG1( "OTA hostname = %s\n", OTA_HOSTNAME );
  LOG1( "ESP chip id = %08x\n", ESP.getChipId());

  watchdogCounter = 0;
  watchdogTicker.attach( 1, ISRwatchdog );

  wifiStationModeStart();
  digitalWrite( WIFI_LED, 1 );
  otaSetup();
  initWebServer();

#ifdef ALEXA_ENABLE
  fauxmo.addDevice(ALEXA_DEVICENAME);
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
      Serial.printf("(%ld) Device #%d (%s) state: %s\n", millis(), device_id, device_name, state ? "ON" : "OFF");
      powerIsOn = state ^ true;
      buttonPressed = true;
  });

  fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
      return powerIsOn;
  });
#endif

  wifiConnected = true;
}

void loop() {

  if( buttonPressed )
  {
    buttonPressed = false;
    toggleRelay();
    if( wifiConnected )
    {
      openHabSendValue((powerIsOn) ? "ON" : "OFF" );
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnected = false;
    digitalWrite( WIFI_LED, LOW );
    Serial.println();
    LOG0("*** WiFi connection lost! ***\n");
    wifiStationModeStart();
    otaSetup();
    initWebServer();
    wifiConnected = true;
  }

  watchdogCounter = 0;
  server.handleClient();
  ArduinoOTA.handle();
  
#ifdef ALEXA_ENABLE
  fauxmo.handle();
#endif
}
