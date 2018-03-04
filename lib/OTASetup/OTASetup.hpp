#ifndef __OTA_SETUP_HPP__
#define __OTA_SETUP_HPP__

#define OTA_CONFIG_FILE_NAME "/otacfg.json"
#define OTA_CONFIG_FILE_MAX_SIZE (256)

#include <JsonConfig.hpp>

class OTASetup : public JsonConfig
{
private:
  char hostname[65];
  char password[65];

public:
  OTASetup();

  const char* getHostname();
  const char* getPassword();

  void setHostname( const char* value );
  void setPassword( const char* value );

  void load();
  void setup();
  void write();
  void print();
};

extern OTASetup otaSetup;

#endif
