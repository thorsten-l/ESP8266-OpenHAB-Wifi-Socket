#include <Arduino.h>
#include <ESP.h>
#include <log.hpp>
#include <PrivateConfig.h>
#include <fauxmoESP.h>
#include <Global.hpp>
#include "AlexaHandler.hpp"

static fauxmoESP fauxmo;
AlexaHandler alexaHandler;

AlexaHandler::AlexaHandler() : JsonConfig( ALEXA_HANDLER_FILE_NAME, ALEXA_HANDLER_FILE_MAX_SIZE )
{
  enabled = ALEXA_ENABLED;
  strcpy( deviceName, ALEXA_DEVICENAME );
}

const char* AlexaHandler::getDeviceName()
{
  return deviceName;
}

const bool AlexaHandler::isEnabled()
{
  return enabled;
}

void AlexaHandler::setDeviceName(const char *value)
{
  strncpy( deviceName, value, 64 );
  deviceName[64] = 0;
}

void AlexaHandler::setEnabled( bool value)
{
  enabled = value;
}

bool AlexaHandler::load()
{
  bool success = true;

  if( exists() )
  {
    LOG0("Loading Alexa config ...\n");

    if ( !JsonConfig::load())
    {
      Serial.println("ERROR: Failed to load wifi config");
      success = false;
    }
    else
    {
      strcpy( deviceName, get("device.name"));
      enabled = getBool("enabled");
      print();
    }
  }
  else
  {
    LOG0("WARNING: Alexa config not exists using default values.\n");
    write();
  }

  return success;
}

void AlexaHandler::setup()
{
  if ( enabled && load() )
  {
    fauxmo.addDevice(ALEXA_DEVICENAME);
    fauxmo.enable(true);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state)
    {
      Serial.printf("(%ld) Device #%d (%s) state: %s\n", millis(), device_id, device_name, state ? "ON" : "OFF");
      powerIsOn = state ^ true;
      buttonPressed = true;
    });

    fauxmo.onGetState([](unsigned char device_id, const char * device_name)
    {
      return powerIsOn;
    });
  }
}

void AlexaHandler::handle()
{
  if( enabled )
  {
    fauxmo.handle();
  }
}

void AlexaHandler::write()
{
  LOG0("Writing Alexa config\n");
  JsonConfig::init();
  set( "device.name", deviceName );
  setBool( "enabled",  enabled );
  JsonConfig::write();
}

void AlexaHandler::print()
{
  Serial.println( "\n-------------------------------------------------------------------------------\nAlexaHandler:");
  Serial.print("  deviceName = ");
  Serial.println(deviceName);
  Serial.print("  enabled = ");
  Serial.println(( enabled ) ? "true" : "false");
  Serial.println("-------------------------------------------------------------------------------\n");
}
