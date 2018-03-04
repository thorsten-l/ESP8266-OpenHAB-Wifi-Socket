#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <log.hpp>
#include <Global.hpp>
#include <PrivateConfig.h>
#include <OTASetup.hpp>
#include <WebServerHandler.hpp>
#include <WifiConfig.hpp>
#include "WifiHandler.hpp"

WifiHandler wifiHandler;

void ISRwatchdog()
{
  if ( watchdogCounter++ == 120 )
  {
    Serial.println();
    LOG0( "*** RESET by watchdog ***\n" );
    restartSystem();
  }
}

void wifiStationModeStart()
{
  LOG0("Starting Wifi in Station Mode");
  WiFi.begin(wifiConfig.getSsid(), wifiConfig.getPassword());
  int status;
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  while (( status = WiFi.status()) != WL_CONNECTED)
  {
    digitalWrite( WIFI_LED, 1 );
    delay(250);
    digitalWrite( WIFI_LED, 0 );
    delay(250);
    Serial.print(".");
  }

  wifiConnected = true;

  Serial.print("\n\nConnected to ");
  Serial.println(wifiConfig.getSsid());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void WifiHandler::setup()
{
  LOG0("WiFi Setup started...");

  wifiConnected = false;
  isWifiInStationMode = false;

  if ( wifiConfig.exists() )
  {
    LOG0( "wifi config already exists.\n");
  }
  else
  {
    LOG0( "wifi config file not exist ... creating default config.\n");
    wifiConfig.init();
    wifiConfig.write();
  }

  ////

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.setOutputPower(0.0f);

  ////

  wifiConfig.load();
  wifiConfig.print();

  WiFi.hostname(otaSetup.getHostname());

  watchdogCounter = 0;
  watchdogTicker.attach( 1, ISRwatchdog );

  if ( strcmp( "AP", wifiConfig.getMode()) == 0 )
  {
    LOG0("Starting Wifi Access Point Mode\n");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig( IPAddress( 192, 168, 192, 1 ),
                       IPAddress( 192, 168, 192, 1 ),
                       IPAddress( 255, 255, 255, 0 ));
    WiFi.softAP( wifiConfig.getSsid(), wifiConfig.getPassword() );
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
  else
  {
    isWifiInStationMode = true;
    WiFi.mode(WIFI_STA);
    wifiStationModeStart();
  }

  wifiConnected = true;

  Serial.print( "wifi hostname = ");
  Serial.println( otaSetup.getHostname());
  Serial.print( "wifi mac address = ");
  Serial.println( WiFi.macAddress());
  Serial.print( "esp core version = ");
  Serial.println( ESP.getCoreVersion());
  Serial.print( "esp sdk version = ");
  Serial.println( ESP.getSdkVersion());
  Serial.println();

  if (MDNS.begin( otaSetup.getHostname(), WiFi.localIP()))
  {
    LOG0("MDNS responder started\n");
  }

  digitalWrite(WIFI_LED,1);
  LOG0("Wifi Setup finished.\n");
}

void WifiHandler::handle()
{
  watchdogCounter = 0;

  if ( isWifiInStationMode )
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println();
      LOG0("*** WiFi connection lost! ***\n");
      wifiStationModeStart();

      webServerHandler.setup();

      if (MDNS.begin(otaSetup.getHostname()))
      {
        LOG0("MDNS responder started\n");
      }
    }
  }
}

bool WifiHandler::isInStationMode()
{
  return isWifiInStationMode;
}
