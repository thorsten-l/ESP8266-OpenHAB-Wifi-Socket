#include <Arduino.h>
#include <ESP.h>
#include <log.hpp>
#include "WifiConfig.hpp"

char ssidBuffer[256];
WifiConfig wifiConfig;

WifiConfig::WifiConfig() : JsonConfig( WIFI_CONFIG_FILE_NAME, WIFI_CONFIG_FILE_MAX_SIZE )
{
  success = false;
  init();
}

const char* WifiConfig::getSsid()
{
  sprintf( ssidBuffer, ssid, ESP.getChipId());
  return ssidBuffer;
}

const char* WifiConfig::getPassword()
{
  return password;
}

const char* WifiConfig::getMode()
{
  return mode;
}

const char* WifiConfig::getAdminPwd()
{
  return adminPwd;
}

void WifiConfig::setSsid(const char *value)
{
  strncpy( ssid, value, 31 );
  ssid[31] = 0;
}

void WifiConfig::setPassword(const char *value)
{
  strncpy( password, value, 64 );
  ssid[64] = 0;
}

void WifiConfig::setMode(const char *value)
{
  strncpy( mode, value, 4 );
  ssid[4] = 0;
}

void WifiConfig::setAdminPwd(const char *value)
{
  strncpy( adminPwd, value, 31 );
  ssid[31] = 0;
}


bool WifiConfig::load()
{
  success = false;

  if ( !JsonConfig::load())
  {
    Serial.println("ERROR: Failed to load wifi config");
  }
  else
  {
    strcpy( ssid, get("ssid"));
    strcpy( password, get("password"));
    strcpy( mode, get("mode"));
    strcpy( adminPwd, get("adminPwd"));
    success = true;
  }

  return success;
}

void WifiConfig::init()
{
  strcpy( ssid, "esp8266-%08x" );
  strcpy( password, "12345678" );
  strcpy( mode, "AP" );
  strcpy( adminPwd, "admin" );
}

void WifiConfig::write()
{
  LOG0("Writing WiFi config\n");
  JsonConfig::init();
  set( "ssid", ssid );
  set( "password", password );
  set( "mode", mode );
  set( "adminPwd", adminPwd );
  JsonConfig::write();
}

void WifiConfig::print()
{
  Serial.println( "\n-------------------------------------------------------------------------------\nWifiConfig:");
  Serial.print("  ssid = ");
  Serial.println(getSsid());
  Serial.print("  password = ");
  Serial.println(password);
  Serial.print("  mode = ");
  Serial.println(mode);
  Serial.print("  adminPwd = ");
  Serial.println(adminPwd);
  Serial.println("-------------------------------------------------------------------------------\n");
}
