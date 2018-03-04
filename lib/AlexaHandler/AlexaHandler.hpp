#ifndef __ALEXA_HANDLER_H__
#define __ALEXA_HANDLER_H__

#define ALEXA_HANDLER_FILE_NAME "/alexacfg.json"
#define ALEXA_HANDLER_FILE_MAX_SIZE (512)

#include "JsonConfig.hpp"

class AlexaHandler : public JsonConfig
{
private:
  char deviceName[65];
  bool enabled;

public:
  AlexaHandler();

  const char* getDeviceName();
  const bool isEnabled();

  void setDeviceName( const char* value );
  void setEnabled( bool value );

  void setup();
  void handle();
  bool load();
  void write();
  void print();
};

extern AlexaHandler alexaHandler;

#endif
