#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include <ArduinoJson.h>

#define JSON_FILE_BUFFER_SIZE 512

class JsonConfig
{
private:
  const char* filename;
  const char* fileBuffer;

  DynamicJsonBuffer jsonBuffer;
  JsonObject* json;

public:
  JsonConfig( const char* _filename );

  const char* get(const char *name);
  const unsigned int getUInt(const char *name);
  const unsigned int getUInt(const char *name, const unsigned int defvalue );
  const bool getBool(const char *name);

  void set( const char *name, const char* value );
  void setUInt( const char *name, const unsigned int value );
  void setBool( const char *name, const bool value );

  bool exists();
  bool load();
  void init();
  void write();
  void print();
  void formatSPIFFS();
};

#endif
