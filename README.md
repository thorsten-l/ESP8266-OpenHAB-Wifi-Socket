# ESP8266-OpenHAB-Wifi-Socket
Alternative Firmware for https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706

Since version 1.1.0 the WiFi Socket firmware can communicate direct with Amazon Alexa (Echo).

## Compile
This is a [PlatformIO](https://platformio.org/) project.

First rename and edit

- `platformio.ini.sample` to `platformio.ini`
- `lib/private/PrivateConfig.h.sample` to `lib/private/PrivateConfig.h`

## Default Settings

|    | value | description |
|----|-------|-------------|
| WiFi SSID | esp8266-`esp id` | SSID in accesspoint mode |
| IP Address | 192.168.192.1 | ip address in accesspoint mode |
| Admin password | admin | Setup admin password |
| OTA Hostname | wifi-socket-`esp id` | *O*ver *T*he *A*ir hostname |
| OTA Password | otapass | OTA firmware upload password |
| OpenHAB Callback Enabled | false | |
| OpenHAB Item Name | pOD_Licht | |
| OpenHAB Host | 192.168.1.1 | |
| OpenHAB Port | 80 | |
| OpenHAB Use Authentication | false | |
| OpenHAB Credentials | SGFsbG86V2VsdA== | user=user, pass=user |
| Alexa enabled | true |  |
| Alexa devicename | Steckdose | Devicename for Alexa HA |

## Hardware
[OBI Wifi Stecker Schuko](https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706)

## OpenHAB - HTTP Binding sample

```
Switch pOD_Licht "Ein / Aus" {http=">[ON:GET:http://wifi-socket.ip/on] >[OFF:GET:http://wifi-socket.ip/off]"}
```

If you like to read the state of the socket by OpenHAB. First create a file `getState.js` in the `transform` directory with the following content:
```
JSON.parse(input).state;
```

add
```
<[http://wifi-socket.ip/state:30000:JS(getState.js)]
```
to the `Switch` definition. Every 30s the state of the socket will be checked.
