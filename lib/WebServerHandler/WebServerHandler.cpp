#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP.h>
#include <log.hpp>
#include <PrivateConfig.h>
#include <fauxmoESP.h>
#include <Global.hpp>
#include <OTASetup.hpp>
#include <AlexaHandler.hpp>
#include <OpenHabHandler.hpp>
#include <WifiConfig.hpp>
#include "WebServerHandler.hpp"
#include "SetupPage.h"

WebServerHandler webServerHandler;
ESP8266WebServer server(80);
char buffer[4096];

void jsonStatus()
{
  String message = "{\"state\":";
  message += ( powerIsOn ) ? "1}\r\n" : "0}\r\n";
  server.send( 200, "application/json", message );
}

void handleSetup()
{
  webServerHandler.setDoRestart( false );

  if ( !server.authenticate( "admin", wifiConfig.getAdminPwd() ))
  {
    server.requestAuthentication();
    return;
  }

  server.arg( "apmode" ).toCharArray( buffer, 4 );

  if ( strcmp( "AP", buffer ) == 0 ||
       strcmp( "STA", buffer ) == 0 )
  {
    wifiConfig.setMode(buffer);
    server.arg( "ssid" ).toCharArray( buffer, 31 );
    wifiConfig.setSsid(buffer);
    server.arg( "password" ).toCharArray( buffer, 64 );
    wifiConfig.setPassword(buffer);
    server.arg( "adminpwd" ).toCharArray( buffer, 31 );
    wifiConfig.setAdminPwd(buffer);

    server.arg( "otahost" ).toCharArray( buffer, 64 );
    otaSetup.setHostname(buffer);
    server.arg( "otapass" ).toCharArray( buffer, 64 );
    otaSetup.setPassword(buffer);

    openHabHandler.setEnabled( server.arg("ohabenabled").length() > 0 );
    openHabHandler.setAuthentication( server.arg("ohabauth").length() > 0 );
    server.arg( "ohabitem" ).toCharArray( buffer, 64 );
    buffer[64]=0;
    openHabHandler.setItemName(buffer);
    server.arg( "ohabhost" ).toCharArray( buffer, 64 );
    buffer[64]=0;
    openHabHandler.setHost(buffer);
    server.arg( "ohabcred" ).toCharArray( buffer, 64 );
    buffer[64]=0;
    openHabHandler.setCredentials(buffer);
    server.arg( "ohabport" ).toCharArray( buffer, 8 );
    buffer[8]=0;
    openHabHandler.setPort( atoi(buffer));

    alexaHandler.setEnabled( server.arg("axenabled").length() > 0 );
    server.arg( "axname" ).toCharArray( buffer, 64 );
    alexaHandler.setDeviceName(buffer);

    webServerHandler.setDoRestart( true );
  }

  char networkBuffer[1024];
  networkBuffer[0] = 0;


  Serial.println( "\nScanning WiFi networks...");
  int n = WiFi.scanNetworks(false, false);
  Serial.println( "done.");

  if (n == 0)
  {
    Serial.println("no networks found");
    strcpy( networkBuffer, "no networks found" );
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    int l = 0;

    for (int i = 0; i < n; ++i)
    {
      l += sprintf( networkBuffer+l, "%2d: %s (%d)%s\n", i+1,
                    WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                    (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay( 10 );
    }
  }

  Serial.println("----------------------------------------------");
  Serial.print(networkBuffer);
  Serial.println("----------------------------------------------");

  isWifiInStationMode = ( strcmp( "STA", wifiConfig.getMode() ) == 0 );

  sprintf( buffer, setupPage, networkBuffer,
           ( isWifiInStationMode ) ? "" : "selected",
           ( isWifiInStationMode ) ? "selected" : "", wifiConfig.getSsid(),
           wifiConfig.getPassword(), wifiConfig.getAdminPwd(),

           otaSetup.getHostname(),
           otaSetup.getPassword(),

           (openHabHandler.isEnabled()) ? "checked" : "",
           openHabHandler.getItemName(),
           openHabHandler.getHost(),
           openHabHandler.getPort(),
           (openHabHandler.useAuthentication()) ? "checked" : "",
           openHabHandler.getCredentials(),

           (alexaHandler.isEnabled()) ? "checked" : "",
           alexaHandler.getDeviceName(),

           ( webServerHandler.shouldDoRestart() ) ? "Configuration saved - restarting the system" : "" );
  String message = String(buffer);

  server.send( 200, "text/html", message );

  if ( webServerHandler.shouldDoRestart() )
  {
    wifiConfig.write();
    otaSetup.write();
    openHabHandler.write();
    alexaHandler.write();
    delay(250);
    server.close();
  }
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
  String message = "<html><body><h1>OpenHAB/Alexa WiFi Socket (";
  message += otaSetup.getHostname();
  message +=  ")</h1>";
  message += "<h4>Version " APP_VERSION " by " APP_AUTHOR " </h4>";

  message += "<h4>Current State</h4>power is ";
  message += ( powerIsOn ) ? "on" : "off";
  message += "<h4>Actions</h4><a href='on'>on</a>";
  message += "<br/><a href='off'>off</a>";
  message += "<br/><a href='state'>state</a>";
  message += "<br/><a href='setup'>setup</a>";

  message += "<h4>Information</h4>";

  message += "Build date = " __DATE__ " " __TIME__ "<br/>";

  message += "OpenHAB use authentication = ";
  message += ( openHabHandler.useAuthentication()) ? "true" : "false";
  message += "<br/>OpenHAB callback enabled = ";
  message += ( openHabHandler.isEnabled()) ? "true" : "false";
  message += "<br/>OpenHAB item name = ";
  message += openHabHandler.getItemName();

  message += "<br/>Alexa enabled = ";
  message += ( alexaHandler.isEnabled()) ? "true" : "false";
  message += "<br/>Alexa device name = ";
  message += alexaHandler.getDeviceName();

  message += "<br/></body></html>";
  server.send( 200, "text/html", message );
}

WebServerHandler::WebServerHandler()
{
  //
}

void WebServerHandler::setup()
{
  server.on("/", handleRoot);
  server.on("/state", []()
  {
    jsonStatus();
  });

  server.on("/on", []()
  {
    powerIsOn = false;
    toggleRelay();
    jsonStatus();
  });

  server.on("/off", []()
  {
    powerIsOn = true;
    toggleRelay();
    jsonStatus();
  });

  server.onNotFound(handleNotFound);

  server.on( "/setup", handleSetup );

  server.begin();
  LOG0("HTTP server started\n");
  MDNS.addService("http", "tcp", 80);
}

const bool WebServerHandler::shouldDoRestart()
{
  return doRestart;
}

void WebServerHandler::setDoRestart( const bool value )
{
  doRestart = value;
}

void WebServerHandler::handle()
{
  server.handleClient();
}
