#include <Arduino.h>
#include <ESP.h>
#include <WiFiClient.h>
#include <Global.hpp>
#include <PrivateConfig.h>
#include <PubSubClient.h>
#include "MqttHandler.hpp"

MqttHandler mqttHandler;

static WiFiClient wifiClient;
static PubSubClient client(wifiClient);
static long lastReconnectAttempt = 0;

static void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();

  if ( length == 1 )
  {
    if ( payload[0] == '1' )
    {
      relayOn();
    }

    if ( payload[0] == '0' )
    {
      relayOff();
    }
  }
  else
  if ( length == 2 )
  {
    if (( payload[0] == 'o' || payload[0] == 'O' )
      && ( payload[1] == 'n' || payload[1] == 'N' ))
    {
      relayOn();
    }
  }
  else
  if ( length == 3 )
  {
    if (( payload[0] == 'o' || payload[0] == 'O' )
      && ( payload[1] == 'f' || payload[1] == 'F' )
      && ( payload[2] == 'f' || payload[2] == 'F' ))
    {
      relayOff();
    }
  }
}

MqttHandler::MqttHandler() : JsonConfig( MQTT_HANDLER_FILE_NAME )
{
  enabled = MQTT_ENABLED;
  strcpy( clientid, MQTT_CLIENTID );
  strcpy( host, MQTT_HOST );
  port = MQTT_PORT;
  authentication = MQTT_USE_AUTHENTICATION;
  strcpy( user, MQTT_USER );
  strcpy( password, MQTT_PASSWORD );
  strcpy( inTopic, MQTT_IN_TOPIC );
  strcpy( outTopic, MQTT_OUT_TOPIC );
}

bool MqttHandler::reconnect()
{
  bool con = false;

  if( authentication )

  if ( (  authentication && client.connect(clientid, user, password ))
    || ( !authentication && client.connect(clientid)))
  {
    LOG0( "mqtt broker connected\n" );
    client.subscribe( inTopic );
  }

  return client.connected();
}

// Getter

const bool MqttHandler::isEnabled()
{
  return enabled;
}

const char* MqttHandler::getClientId()
{
  return clientid;
}

const char* MqttHandler::getHost()
{
  return host;
}


const int MqttHandler::getPort()
{
  return port;
}

const bool MqttHandler::useAuthentication()
{
  return authentication;
}

const char* MqttHandler::getUser()
{
  return user;
}

const char* MqttHandler::getPassword()
{
  return password;
}

const char* MqttHandler::getInTopic()
{
  return inTopic;
}

const char* MqttHandler::getOutTopic()
{
  return outTopic;
}

// Setter

void MqttHandler::setEnabled( const bool value )
{
  enabled = value;
}

void MqttHandler::setClientId(const char *value)
{
  strncpy( clientid, value, 64 );
  host[64] = 0;
}

void MqttHandler::setHost(const char *value)
{
  strncpy( host, value, 64 );
  host[64] = 0;
}

void MqttHandler::setPort(const int value)
{
  port = value;
}

void MqttHandler::setAuthentication( const bool value )
{
  authentication = value;
}

void MqttHandler::setUser(const char *value)
{
  strncpy( user, value, 64 );
  user[64] = 0;
}

void MqttHandler::setPassword(const char *value)
{
  strncpy( password, value, 64 );
  password[64] = 0;
}

void MqttHandler::setInTopic(const char *value)
{
  strncpy( inTopic, value, 64 );
  inTopic[64] = 0;
}

void MqttHandler::setOutTopic(const char *value)
{
  strncpy( outTopic, value, 64 );
  outTopic[64] = 0;
}



////////

void MqttHandler::setup()
{
  if ( exists())
  {
    if ( !JsonConfig::load())
    {
      LOG0("ERROR: Failed to load MQTT config");
    }
    else
    {
      LOG0("MQTT config loaded.\n");
      enabled = getBool( "enabled" );
      strcpy( clientid, get("clientid"));
      strcpy( host, get("host"));
      port = getUInt( "port", MQTT_PORT );
      authentication = getBool( "auth" );
      strcpy( user, get("user"));
      strcpy( password, get("password"));
      strcpy( inTopic, get("intopic"));
      strcpy( outTopic, get("outtopic"));
    }
  }
  else
  {
    LOG0("WARNING: MQTT config not exists using default values.\n");
    write();
  }

  client.setServer( host, port );
  client.setCallback( callback );
  print();
}

void MqttHandler::write()
{
  LOG0("Writing MQTT config\n");
  JsonConfig::init();

  setBool( "enabled", enabled );
  set( "clientid", clientid );
  set( "host", host );
  setUInt( "port", port );
  setBool( "auth", authentication );
  set( "user", user );
  set( "password", password );
  set( "intopic", inTopic );
  set( "outtopic", outTopic );

  JsonConfig::write();
}

void MqttHandler::print()
{
  Serial.println( "\n-------------------------------------------------------------------------------\nMqttHandler:");
  Serial.print("  enabled = ");
  Serial.println((enabled) ? "true" : "false" );
  Serial.print("  client id = ");
  Serial.println(clientid);
  Serial.print("  host = ");
  Serial.println(host);
  Serial.print("  port = ");
  Serial.println(port);
  Serial.print("  use authentication = ");
  Serial.println((authentication) ? "true" : "false" );
  Serial.print("  user = ");
  Serial.println(user);
  Serial.print("  password = ");
  Serial.println(password);
  Serial.print("  in topic = ");
  Serial.println(inTopic);
  Serial.print("  out topic = ");
  Serial.println(outTopic);
  Serial.println("-------------------------------------------------------------------------------\n");
}

void MqttHandler::handle()
{
  if (!client.connected())
  {
    long now = millis();

    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;

      // Attempt to reconnect
      if (reconnect())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    client.loop();
  }
}

void MqttHandler::sendValue( const char* value )
{
  if( enabled && client.connected())
  {
    client.publish( outTopic, value );
  }
}
