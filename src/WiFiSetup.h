#ifndef WiFiSetup_h
#define WiFiSetup_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define WIFISETUP_BUFFER_SIZE 32
#define WIFISETUP_DATA_SIZE 512
#define WIFISETUP_DATA_FLAG_SIZE 1

#define PRINT_DEBUG 1

#if PRINT_DEBUG
#define DPRINTLN(s)  { Serial.println(s); }
#define DPRINTS(s)   { Serial.print(s); }
#define DPRINT(s,v)  { Serial.print(s); Serial.print(v); }
#define DPRINTX(s,x) { Serial.print(s); Serial.print(v, HEX); }
#else
#define DPRINTLN(s)
#define DPRINTS(s)
#define DPRINT(s,v)
#define DPRINTX(s,x)
#endif

// EEPROM:
// ----------------------------------------------
// |  1 byte  | 32 byte | 32 byte | 447 byte    |
// ----------------------------------------------
// |  flag    | SSID    | Passwd  | Userdefine  |
// ----------------------------------------------

class WiFiSetup {
 private:
  char hotspotSSID[WIFISETUP_BUFFER_SIZE];
  char hotspotPasswd[WIFISETUP_BUFFER_SIZE];
  bool hasPasswd;
  bool needSetupCached;
  bool isNeedSetupCached;
  bool needRestartCached;
  int ledPin;
  std::unique_ptr<ESP8266WebServer> server;

  void writeWiFiData(const char *ssid, const char *passwd);
  void readWiFiData(char *ssid, char *passwd);
  void turnLEDOn();
  void turnLEDOff();

 public:
  WiFiSetup(int ledPin, char *ssid = nullptr, char *passwd = nullptr);

  void reset();
  bool needSetup();
  bool needRestart();
  int prepareServing();
  int tryConnectingWiFi();
  int serveWeb();
  // if true, return;
  bool autoSetup();
  // if true, return
  bool autoLoop();
};


#endif
