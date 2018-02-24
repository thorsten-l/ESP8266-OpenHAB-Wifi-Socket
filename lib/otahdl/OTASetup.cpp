#include <Arduino.h>
#include <ArduinoOTA.h>

#include <log.hpp>
#include <PrivateConfig.h>

#include "OTASetup.hpp"

void otaSetup()
{
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((char*)OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("\nOTA Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End\n");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("\rOTA Progress: %u%%", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
  });

  ArduinoOTA.begin();
}
