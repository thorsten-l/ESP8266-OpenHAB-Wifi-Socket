#ifndef __SETUP_PAGE_H__
#define __SETUP_PAGE_H__

const char *setupPage =
"<html>"
  "<head>"
    "<title>Setup</title>"
  "</head>"
  "<body>"
    "<h1>Setup</h1>"
    "<h2>WiFi Network Scan</h2>"
    "<pre>%s</pre>"
    "<form method='GET'>"

      "<h2>WiFi</h2>"
      "<p>Mode <select name='apmode'>"
        "<option %s value='AP'>Access Point</option>"
        "<option %s value='STA'>Station</option>"
      "</select></p>"
      "<p>SSID <input type='text' name='ssid' size='34' maxlength='30' value='%s'></p>"
      "<p>Password <input type='text' name='password' size='68' maxlength='64' value='%s'></p>"

      "<h2>Security</h2>"
      "<p>Admin Password <input type='text' name='adminpwd' size='34' maxlength='30' value='%s'></p>"

      "<h2>Over the air (OTA)</h2>"
      "<p>OTA Hostname <input type='text' name='otahost' size='68' maxlength='64' value='%s'></p>"
      "<p>OTA Password <input type='text' name='otapass' size='68' maxlength='64' value='%s'></p>"

      "<h2>OpenHAB</h2>"
      "<p>OpenHAB Callback Enabled <input type='checkbox' name='ohabenabled' value='true' %s><p>"
      "<p>OpenHAB Host <input type='text' name='ohabhost' size='68' maxlength='64' value='%s'></p>"
      "<p>OpenHAB Port <input type='text' name='ohabport' size='68' maxlength='64' value='%d'></p>"
      "<p>OpenHAB Use Authentication <input type='checkbox' name='ohabauth' value='true' %s><p>"
      "<p>OpenHAB Credentials <input type='text' name='ohabcred' size='68' maxlength='64' value='%s'> <a target='_blank' href='https://www.blitter.se/utils/basic-authentication-header-generator/'>Generator</a></p>"

      "<h2>Alexa</h2>"
      "<p>Alexa Enabled <input type='checkbox' name='axenabled' value='true' %s><p>"
      "<p>Alexa Devicename <input type='text' name='axname' size='68' maxlength='64' value='%s'></p>"

      "<p><input type='submit' value='Save'></p>"
    "</form>"
    "<p>%s</p>"
  "</body>"
"</html>";

#endif
