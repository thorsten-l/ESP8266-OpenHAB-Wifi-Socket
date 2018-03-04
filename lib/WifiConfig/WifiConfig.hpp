#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

#define WIFI_CONFIG_FILE_NAME "/wificfg.json"
#define WIFI_CONFIG_FILE_MAX_SIZE (512)

#include "JsonConfig.hpp"

class WifiConfig : public JsonConfig
{
private:
  char ssid[32];
  char password[65];
  char mode[5];
  char adminPwd[32];
  bool success;

public:
  WifiConfig();

  const char* getSsid();
  const char* getPassword();
  const char* getMode();
  const char* getAdminPwd();

  void setSsid( const char* value );
  void setPassword( const char* value );
  void setMode( const char* value );
  void setAdminPwd( const char* value );

  bool load();
  void init();
  void write();
  void print();
};

extern WifiConfig wifiConfig;

#endif
