#ifndef __OPENHAB_HANDLER_H__
#define __OPENHAB_HANDLER_H__

#define OPENHAB_HANDLER_FILE_NAME "/openhabcfg.json"
#define OPENHAB_HANDLER_FILE_MAX_SIZE (512)

#include "JsonConfig.hpp"

class OpenHabHandler : public JsonConfig
{
private:
  char host[65];
  int port;
  char itemName[65];
  char credentials[65];
  bool authentication;
  bool enabled;

public:
  OpenHabHandler();

  const char* getHost();
  const int getPort();
  const char* getItemName();
  const char* getCredentials();
  const bool useAuthentication();
  const bool isEnabled();

  void setHost( const char* value );
  void setPort( const int value );
  void setItemName( const char* value );
  void setCredentials( const char* value );
  void setAuthentication( const bool value );
  void setEnabled( const bool value );

  void setup();
  void write();
  void print();

  void sendValue( const char* value );
};

extern OpenHabHandler openHabHandler;

#endif
