#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

#include <Arduino.h>
#include <log.hpp>
#include <Ticker.h>

#define WIFI_LED 4
#define POWER_BUTTON 14
#define RELAY_TRIGGER_OFF 5
#define RELAY_TRIGGER_ON 12

extern long timestamp;
extern bool buttonPressed;
extern bool powerIsOn;
extern bool setupDone;
extern bool wifiConnected;
extern bool isWifiInStationMode;
extern int watchdogCounter;

extern Ticker watchdogTicker;

extern void restartSystem();
extern void toggleRelay();

#endif
