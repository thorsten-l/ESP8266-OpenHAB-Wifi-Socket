#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__

#define MQTT_HANDLER_FILE_NAME "/mqttcfg.json"

#include "JsonConfig.hpp"

class MqttHandler : public JsonConfig
{
private:
  bool enabled;
  char clientid[65];
  char host[65];
  int port;
  bool authentication;
  char user[65];
  char password[65];
  char inTopic[65];
  char outTopic[65];

  bool reconnect();

public:
  MqttHandler();

  const bool isEnabled();
  const char* getClientId();
  const char* getHost();
  const int getPort();
  const bool useAuthentication();
  const char* getUser();
  const char* getPassword();
  const char* getInTopic();
  const char* getOutTopic();

  void setEnabled( const bool value );
  void setClientId( const char* value );
  void setHost( const char* value );
  void setPort( const int value );
  void setAuthentication( const bool value );
  void setUser( const char* value );
  void setPassword( const char* value );
  void setInTopic( const char* value );
  void setOutTopic( const char* value );

  void setup();
  void write();
  void print();
  void handle();

  void sendValue( const char* value );
};

extern MqttHandler mqttHandler;

#endif
