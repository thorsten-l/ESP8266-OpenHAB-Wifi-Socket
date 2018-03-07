#include <Arduino.h>
#include <ArduinoOTA.h>
#include <log.hpp>
#include <PrivateConfig.h>
#include <Global.hpp>
#include <ESP8266mDNS.h>
#include "OTASetup.hpp"

char hostnameBuffer[96];
OTASetup otaSetup;

OTASetup::OTASetup() : JsonConfig( OTA_CONFIG_FILE_NAME, OTA_CONFIG_FILE_MAX_SIZE )
{
  strcpy( hostname, OTA_HOSTNAME );
  strcpy( password, OTA_PASSWORD );
}

const char* OTASetup::getHostname()
{
  sprintf( hostnameBuffer, hostname, ESP.getChipId());
  return hostnameBuffer;
}

const char* OTASetup::getPassword()
{
  return password;
}

void OTASetup::setHostname(const char *value)
{
  strncpy( hostname, value, 64 );
  hostname[64] = 0;
}

void OTASetup::setPassword(const char *value)
{
  strncpy( password, value, 64 );
  password[64] = 0;
}

void OTASetup::load()
{
  if( exists() )
  {
    LOG0("Loading OTA config ...\n");

    if ( !JsonConfig::load())
    {
      LOG0("ERROR: Failed to load ota config\n");
    }
    else
    {
      strcpy( hostname, get("hostname"));
      strcpy( password, get("password"));
      LOG0("done.");
    }
  }
  else
  {
    LOG0("WARNING: OTA config not exists using default values.\n");
    write();
  }

  print();
}

void OTASetup::setup()
{
  ArduinoOTA.setHostname(getHostname());
  ArduinoOTA.setPassword(password);

  ArduinoOTA.onStart([]()
  {
    Serial.println("\nOTA Start");
  });

  ArduinoOTA.onEnd([]()
  {
    digitalWrite(WIFI_LED,0);
    Serial.println("\nOTA End\n");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("\rOTA Progress: %u%%", (progress / (total / 100)));
    digitalWrite(WIFI_LED, digitalRead(WIFI_LED) ^ true);
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("OTA Error[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
      Serial.println("OTA Auth Failed");
    else
    if (error == OTA_BEGIN_ERROR)
      Serial.println("OTA Begin Failed");
    else
    if (error == OTA_CONNECT_ERROR)
      Serial.println("OTA Connect Failed");
    else
    if (error == OTA_RECEIVE_ERROR)
      Serial.println("OTA Receive Failed");
    else
    if (error == OTA_END_ERROR)
      Serial.println("OTA End Failed");
  });

  ArduinoOTA.begin();
  MDNS.addServiceTxt("arduino", "tcp", "fw_name", APP_NAME );
  MDNS.addServiceTxt("arduino", "tcp", "fw_version", APP_VERSION );
}

void OTASetup::write()
{
  LOG0("Writing OTA config\n");
  JsonConfig::init();
  set( "hostname", hostname );
  set( "password", password );
  JsonConfig::write();
}

void OTASetup::print()
{
  Serial.println( "\n-------------------------------------------------------------------------------\nOTAConfig:");
  Serial.print("  hostname = ");
  Serial.println(getHostname());
  Serial.print("  password = ");
  Serial.println(password);
  Serial.println("-------------------------------------------------------------------------------\n");
}
