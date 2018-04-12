#include "WiFiSetup.h"

#define NOMINMAX
#if defined NOMINMAX

#if defined min
#undef min
#endif
template <class T>
inline const T& __attribute__((always_inline)) min(const T& a, const T& b) {
  return !(b < a) ? a : b;
}

#endif

const char *serverIndex = "<form action=\"/update\" method=\"post\">SSID:<input type=\"text\" name=\"ssid\"><br>password:<input type=\"text\" name=\"passwd\"><br><input type=\"submit\" value=\"Submit\"></form>";


// WiFiSetup
WiFiSetup::WiFiSetup(int ledPin, char *ssid, char *passwd) {
  this->ledPin = ledPin;
  if (ssid == nullptr) {
    strcpy(this->hotspotSSID, "ConnectMe");
  } else {
    strncpy(this->hotspotSSID, ssid, min<int>(WIFISETUP_BUFFER_SIZE-1, strlen(ssid)));
  }

  if (passwd != nullptr) {
    strncpy(this->hotspotPasswd, passwd, WIFISETUP_BUFFER_SIZE-1);
    this->hasPasswd = true;
  }

  pinMode(ledPin, OUTPUT);
}

void WiFiSetup::reset() {
  EEPROM.begin(WIFISETUP_DATA_SIZE);
  for(int i = 0; i < WIFISETUP_DATA_SIZE; ++i) {
    EEPROM.write(i, 0);
  }

  EEPROM.end();
}

bool WiFiSetup::needSetup() {
  if (this->isNeedSetupCached) {
    return this->needSetupCached;
  }

  EEPROM.begin(WIFISETUP_DATA_SIZE);
  byte value = EEPROM.read(0);
  EEPROM.end();

  this->isNeedSetupCached = true;
  this->needSetupCached = value == 0;
  return this->needSetupCached;
}

bool WiFiSetup::needRestart() {
  return this->needRestartCached;
}

int WiFiSetup::prepareServing() {
  this->server = ESP8266WebServer(80);
  if (this->hasPasswd) {
    WiFi.softAP(this->hotspotSSID, this->hotspotPasswd);
  } else {
    WiFi.softAP(this->hotspotSSID);
  }

  this->server.on("/", HTTP_GET, [&](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  this->server.on("/update", HTTP_POST, [&](){
    String ssid = server.arg("ssid");
    String passwd = server.arg("passwd");
    this->writeWiFiData(ssid.c_str(), passwd.c_str());
    this->needRestartCached = true;

    server.send(200, "text/html", "Data saved, ready to restart.");
  });

  this->server.begin();
  DPRINTLN("Start serving...");
  return 0;
}

int WiFiSetup::tryConnectingWiFi() {
  char ssid[WIFISETUP_BUFFER_SIZE] = { 0 };
  char passwd[WIFISETUP_BUFFER_SIZE] = { 0 };
  this->readWiFiData(ssid, passwd);

  DPRINTS("Connecting to ");
  DPRINTLN(ssid);

  WiFi.begin(ssid, passwd);
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
    turnLEDOn();
    delay(250);
    turnLEDOff();
    DPRINTS(".");
  }

  turnLEDOn();

  DPRINTLN("");
  DPRINTLN("WiFI connected");
  DPRINTS("IP address: ");
  // DPRINTLN(WiFi.localIP());

  return 0;
}

int WiFiSetup::serveWeb() {
  this->server.handleClient();

  return 0;
}

void WiFiSetup::turnLEDOn() {
  digitalWrite(LED_BUILTIN, LOW);
}

void WiFiSetup::turnLEDOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void WiFiSetup::writeWiFiData(const char *ssid, const char *passwd) {
  EEPROM.begin(WIFISETUP_DATA_SIZE);

  for(int i = 0; i <WIFISETUP_DATA_FLAG_SIZE; ++i) {
    EEPROM.write(i, 1);
  }

  for(int i = WIFISETUP_DATA_FLAG_SIZE;
      i < WIFISETUP_DATA_FLAG_SIZE+WIFISETUP_BUFFER_SIZE; ++i) {
    EEPROM.write(i, ssid[i-WIFISETUP_DATA_FLAG_SIZE]);
  }

  for(int i = WIFISETUP_DATA_FLAG_SIZE + WIFISETUP_BUFFER_SIZE;
      i < WIFISETUP_DATA_FLAG_SIZE + WIFISETUP_BUFFER_SIZE * 2; ++i) {
    EEPROM.write(i, passwd[i - WIFISETUP_DATA_FLAG_SIZE - WIFISETUP_BUFFER_SIZE]);
  }
  EEPROM.end();
}

void WiFiSetup::readWiFiData(char *ssid, char *passwd) {
  EEPROM.begin(WIFISETUP_DATA_SIZE);
  for(int i = WIFISETUP_DATA_FLAG_SIZE;
      i < WIFISETUP_DATA_FLAG_SIZE + WIFISETUP_BUFFER_SIZE; ++i) {
    ssid[i - WIFISETUP_DATA_FLAG_SIZE] = EEPROM.read(i);
  }

  for(int i = WIFISETUP_DATA_FLAG_SIZE + WIFISETUP_BUFFER_SIZE;
      i < WIFISETUP_DATA_FLAG_SIZE + WIFISETUP_BUFFER_SIZE * 2; ++i) {
    passwd[i - WIFISETUP_DATA_FLAG_SIZE - WIFISETUP_BUFFER_SIZE] = EEPROM.read(i);
  }
  EEPROM.end();
}

