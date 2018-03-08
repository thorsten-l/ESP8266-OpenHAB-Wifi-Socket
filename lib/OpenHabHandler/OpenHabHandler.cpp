#include <Arduino.h>
#include <ESP.h>
#include <WiFiClient.h>
#include <Global.hpp>
#include <PrivateConfig.h>
#include "OpenHabHandler.hpp"

OpenHabHandler openHabHandler;

OpenHabHandler::OpenHabHandler() :
                                 JsonConfig( OPENHAB_HANDLER_FILE_NAME, OPENHAB_HANDLER_FILE_MAX_SIZE )
{
  strcpy( host, OHAB_HOST );
  port = OHAB_PORT;
  strcpy( itemName, OHAB_ITEM );
  strcpy( credentials, OHAB_CREDENTIALS );
  authentication = OHAB_USE_AUTHENTICATION;
  enabled = OHAB_ENABLE_CALLBACK;
  version = OHAB_VERSION;
}

const char* OpenHabHandler::getHost()
{
  return host;
}

const bool OpenHabHandler::useAuthentication()
{
  return authentication;
}

const bool OpenHabHandler::isEnabled()
{
  return enabled;
}

const int OpenHabHandler::getPort()
{
  return port;
}

const int OpenHabHandler::getVersion()
{
  return version;
}

const char* OpenHabHandler::getItemName()
{
  return itemName;
}

const char* OpenHabHandler::getCredentials()
{
  return credentials;
}

void OpenHabHandler::setHost(const char *value)
{
  strncpy( host, value, 64 );
  host[64] = 0;
}

void OpenHabHandler::setPort(const int value)
{
  port = value;
}

void OpenHabHandler::setVersion(const int value)
{
  version = value;
}

void OpenHabHandler::setItemName(const char *value)
{
  strncpy( itemName, value, 64 );
  itemName[64] = 0;
}

void OpenHabHandler::setCredentials(const char *value)
{
  strncpy( credentials, value, 64 );
  credentials[64] = 0;
}

void OpenHabHandler::setAuthentication( const bool value )
{
  authentication = value;
}

void OpenHabHandler::setEnabled( const bool value )
{
  enabled = value;
}

void OpenHabHandler::setup()
{
  if ( exists())
  {
    if ( !JsonConfig::load())
    {
      LOG0("ERROR: Failed to load OpenHAB config");
    }
    else
    {
      enabled = getBool( "enabled" );
      strcpy( host, get("host"));
      port = getUInt("port", OHAB_PORT );
      version = getUInt("version", OHAB_VERSION );
      strcpy( itemName, get("item"));
      authentication = getBool( "auth" );
      strcpy( credentials, get("cred"));
    }
  }
  else
  {
    LOG0("WARNING: OpenHAB config not exists using default values.\n");
    write();
  }

  print();
}


void OpenHabHandler::write()
{
  LOG0("Writing OpenHAB config\n");
  JsonConfig::init();
  set( "host", host );
  setBool( "enabled", enabled );
  setUInt( "port", port );
  setUInt( "version", version );
  set( "item", itemName );
  setBool( "auth", authentication );
  set( "cred", credentials );
  JsonConfig::write();
}

void OpenHabHandler::print()
{
  Serial.println( "\n-------------------------------------------------------------------------------\nOpenHabHandler:");
  Serial.print("  host = ");
  Serial.println(host);
  Serial.print("  port = ");
  Serial.println(port);
  Serial.print("  version = ");
  Serial.println(version);
  Serial.print("  item name = ");
  Serial.println(itemName);
  Serial.print("  credentials = ");
  Serial.println(credentials);
  Serial.print("  use authentication = ");
  Serial.println((authentication) ? "true" : "false" );
  Serial.print("  callback enabled = ");
  Serial.println((enabled) ? "true" : "false" );
  Serial.println("-------------------------------------------------------------------------------\n");
}


void OpenHabHandler::sendValueV1( const char* value )
{
  WiFiClient client;

  Serial.println( "Sending to OpenHab V1");
  Serial.print( "openhab host = ");
  Serial.print( host );
  Serial.print( ", port = ");
  Serial.println( port );

  if (!client.connect( host, port ))
  {
    Serial.println("connection failed");
    client.stop();
    return;
  }

  String request = String("GET /CMD?") + itemName + "=" + value + " HTTP/1.1\r\n";

  if( authentication )
  {
    request = request + "Authorization: Basic " + credentials + "\r\n";
  }

  request = request + "Host: " + host + "\r\nConnection: close\r\n\r\n";

  //Serial.println( "\n\n--- Request ---\n" + request );

  client.print(request);
  delay(50);

  //Serial.println( "--- Response ---" );
  // Read all the lines of the reply from server and print them to Serial
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    //Serial.print(line);
  }

  //Serial.println("\nclosing connection");
  client.stop();
}

void OpenHabHandler::sendValueV2( const char* value )
{
  WiFiClient client;

  Serial.println( "Sending to OpenHab V2");
  Serial.print( "openhab host = ");
  Serial.print( host );
  Serial.print( ", port = ");
  Serial.println( port );

  if (!client.connect( host, port ))
  {
    Serial.println("connection failed");
    client.stop();
    return;
  }

  String request = String( "POST /rest/items/" ) + itemName + " HTTP/1.1\r\n";
  request =  request + "Host: " + host + "\r\n";

  request += "Cache-Control: no-cache\r\n";
  request += "Accept: application/json\r\n";
  request += "Content-Type: text/plain\r\n";
  request += "Content-Length: ";
  request += String(strlen(value));
  request +=  "\r\n";
  request += "Connection: close\r\n";

  if( authentication )
  {
    request = request + "Authorization: Basic " + credentials + "\r\n";
  }

  request +=  "\r\n";

  request = request + value;

  // Serial.println( "\n\n--- Request ---\n" + request );

  client.print(request);
  client.flush();
  client.stop();

  // Serial.println( "--- Response ---" );
  // Read all the lines of the reply from server and print them to Serial
  while (client.connected())
  {
    if( client.available())
    {
      String line = client.readStringUntil('\r');
      // Serial.print(line);
    }
  }

  // Serial.println("\nclosing connection");
}


void OpenHabHandler::sendValue( const char* value )
{
  if( enabled )
  {
    if( version == 1 )
    {
      sendValueV1( value );
    }
    else
    {
      sendValueV2( value );
    }
  }
}
