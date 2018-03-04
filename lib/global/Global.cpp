#include "Global.hpp"

long timestamp;
bool buttonPressed;
bool powerIsOn;
bool setupDone;
bool wifiConnected;
bool isWifiInStationMode;
int watchdogCounter;

Ticker watchdogTicker;

void restartSystem()
{
  watchdogTicker.detach();
  ESP.eraseConfig();
  Serial.println( "*** restarting system ***" );
  delay( 2000 );
  ESP.restart();
  delay( 2000 );
  ESP.reset();
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

  if( setupDone )
  {
    LOG1( "toggle power state to: %s\n", (powerIsOn) ? "ON" : "OFF" );
  }
}
