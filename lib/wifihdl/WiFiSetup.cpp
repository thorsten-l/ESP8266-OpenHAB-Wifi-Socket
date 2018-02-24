#include <Arduino.h>
#include <ArduinoOTA.h>

#include <log.hpp>
#include <PrivateConfig.h>

#include "WiFiSetup.hpp"

int wifiled;

void wifiStationModeStart()
{
  LOG0("Starting Wifi in Station Mode");
  wifiled = 0;
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.setOutputPower(0.0f);
  WiFi.hostname(OTA_HOSTNAME);

  delay(500);

  WiFi.begin(WIFI_SSID, WIFI_PWD);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  while (WiFi.status() != WL_CONNECTED)
  {
    wifiled = 1 - wifiled;
    digitalWrite( WIFI_LED, wifiled );
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  LOG1("Connected to %s\n", WIFI_SSID );
  LOG0("IP address: " );
  Serial.println( WiFi.localIP() );
}
