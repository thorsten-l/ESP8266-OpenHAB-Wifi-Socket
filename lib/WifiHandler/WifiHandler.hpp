#ifndef __WIFI_HANDLER_H__
#define __WIFI_HANDLER_H__

class WifiHandler
{

public:
  void setup();
  void handle();
  bool isInStationMode();
};

extern WifiHandler wifiHandler;

#endif
