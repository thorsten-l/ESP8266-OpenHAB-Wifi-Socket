#include <Arduino.h>
#include <FS.h>
#include <ESP.h>
#include <log.hpp>
#include "JsonConfig.hpp"

JsonConfig::JsonConfig( const char* _filename, int _fileBufferSize )  :
  filename(_filename), fileBufferSize(_fileBufferSize)
{
}

const char* JsonConfig::get(const char *name)
{
  //Serial.print( "  - name = " );
  //Serial.println( name );
  const char* value = json->get<const char*>(name);
  //Serial.print( "    char * = " );
  //Serial.println( value );
  return value;
}

const unsigned int JsonConfig::getUInt(const char *name)
{
  //Serial.print( "  - name = " );
  //Serial.println( name );
  unsigned int value = json->get<unsigned int>(name);
  //Serial.print( "    uint = " );
  //Serial.println( value );

  return value;
}

const unsigned int JsonConfig::getUInt(const char *name, const unsigned int defvalue )
{
  unsigned int value = defvalue;

  //Serial.print( "  - name = " );
  //Serial.println( name );
  if( json->containsKey(name))
  {
    value = json->get<unsigned int>(name);
  }
  //Serial.print( "    uint = " );
  //Serial.println( value );

  return value;
}

const bool JsonConfig::getBool(const char *name)
{
  //Serial.print( "  - name = " );
  //Serial.println( name );
  bool value = json->get<bool>(name);
  //Serial.print( "    bool = " );
  //Serial.println( value );
  return value;
}

void JsonConfig::set( const char *name, const char* value )
{
  json->set( name, value );
}

void JsonConfig::setUInt( const char *name, const unsigned int value )
{
  json->set( name, value );
}

void JsonConfig::setBool( const char *name, const bool value )
{
  json->set( name, value );
}

bool JsonConfig::exists()
{
  bool configFileExists = false;

  if (!SPIFFS.begin())
  {
    LOG0("ERROR: Failed to mount file system");
  }
  else
  {
    configFileExists = SPIFFS.exists(filename);
    SPIFFS.end();
  }

  return configFileExists;
}

bool JsonConfig::load()
{
  bool success = false;
  char buffer[fileBufferSize + 1]; // + trailing zero

  if (!SPIFFS.begin())
  {
    LOG0("Failed to mount file system");
  }
  else
  {
    File configFile = SPIFFS.open(filename, "r");

    if (!configFile)
    {
      LOG1("ERROR: file %s not found.\n", filename );
    }
    else
    {
      LOG1("Loading configuration from %s file...\n", filename );

      if( configFile.size() > fileBufferSize )
      {
        Serial.printf( "ERROR: %s file exceeds %d bytes.\n", filename, fileBufferSize);
      }
      else
      {
        int bytesRead = configFile.readBytes(buffer, fileBufferSize );
        LOG1( "%d bytes read from config file.\n", bytesRead );

        buffer[bytesRead] = 0;

/*
        Serial.println("----------------------------------------------------");
        Serial.println( buffer );
        Serial.println("----------------------------------------------------");
        Serial.println();
*/

        json = &jsonBuffer.parseObject(buffer, bytesRead);

        if( !json->success() )
        {
          LOG0( "ERROR: JSON parsing failed.");
        }
        else
        {
          success = true;
        }
      }

      configFile.close();
    }

    SPIFFS.end();
  }

  return success;
}

void JsonConfig::write()
{
  if (!SPIFFS.begin())
  {
    LOG0("Failed to mount file system");
    return;
  }

  Serial.printf("writing file %s.\n", filename );

  File configFile = SPIFFS.open( filename, "w");

  if (!configFile)
  {
    LOG1("ERROR: Failed to open config file %s for writing.\n", filename );
    return;
  }

  json->printTo(configFile);
  configFile.close();
  SPIFFS.end();
}

void JsonConfig::init()
{
  if ( json != 0 )
  {
    json->invalid();
  }

  json = &jsonBuffer.createObject();
}

void JsonConfig::print()
{
  Serial.println();
  json->prettyPrintTo(Serial);
  Serial.println();
}

void JsonConfig::formatSPIFFS()
{
  SPIFFS.begin();
  delay( 200 );
  LOG0("File system format started...");
  SPIFFS.format();
  LOG0("File system format finished.");
  SPIFFS.end();
}
