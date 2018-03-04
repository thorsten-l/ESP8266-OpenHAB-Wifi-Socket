#ifndef __WEB_SERVER_HANDLER_H__
#define __WEB_SERVER_HANDLER_H__

class WebServerHandler
{
private:
  bool doRestart;

public:
  WebServerHandler();

  const bool shouldDoRestart();
  void setup();
  void handle();
  void setDoRestart( const bool value );
};

extern WebServerHandler webServerHandler;

#endif
