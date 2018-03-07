#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <OTASetup.hpp>
#include <Global.hpp>
#include <PrivateConfig.h>
#include <log.hpp>
#include <WifiHandler.hpp>
#include <OTASetup.hpp>
#include <AlexaHandler.hpp>
#include <WebServerHandler.hpp>
#include <OpenHabHandler.hpp>

////////////////////////////////////////////////////////////////////////////////

void powerButtonPressed()
{
  if (( millis() - timestamp > 100 )) // button debouncing
  {
    buttonPressed = true;
  }

  timestamp = millis();
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
  setupDone = false;
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

  for( int i=0; i<5; i++)
  {
    digitalWrite( WIFI_LED, 1 );
    delay(500);
    digitalWrite( WIFI_LED, 0 );
    delay(500);
  }

  Serial.println();
  Serial.println("\n\n");
  Serial.println("\n\n");
  Serial.println(F("OpenHAB/Alexa WiFi Socket " APP_VERSION " - by " APP_AUTHOR ));
  Serial.println( "Build date: " __DATE__ " " __TIME__ "\n" );

  if ( digitalRead(POWER_BUTTON) == false )
  {

    Serial.println();
    Serial.println("*** RESET Configuration ***");
    Serial.println();

    for( int i=0; i<15; i++)
    {
      digitalWrite( WIFI_LED, 1 );
      delay( 100 );
      digitalWrite( WIFI_LED, 0 );
      delay( 100 );
    }

    digitalWrite( WIFI_LED, 1 );

    ESP.eraseConfig();

    if (SPIFFS.begin())
    {
      Serial.println(F("File system format started..."));
      SPIFFS.format();
      Serial.println(F("File system format finished."));
      SPIFFS.end();
    }

    digitalWrite( WIFI_LED, 0 );
    restartSystem();
  }

  ESP.eraseConfig();

  attachInterrupt(digitalPinToInterrupt(POWER_BUTTON), &powerButtonPressed, FALLING);

  delay(500);

  otaSetup.load();
  wifiHandler.setup();
  otaSetup.setup();
  webServerHandler.setup();
  openHabHandler.setup();
  alexaHandler.setup();
  setupDone = true;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  if( buttonPressed )
  {
    buttonPressed = false;
    toggleRelay();

    if( wifiHandler.isInStationMode() && wifiConnected )
    {
      openHabHandler.sendValue((powerIsOn) ? "ON" : "OFF" );
    }
  }

  wifiHandler.handle();
  webServerHandler.handle();
  alexaHandler.handle();
  ArduinoOTA.handle();

  if(webServerHandler.shouldDoRestart())
  {
    restartSystem();
  }
}
