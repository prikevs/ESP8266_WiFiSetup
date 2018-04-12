#include <WiFiSetup.h>

WiFiSetup wst(13, "HelloG");

void setup() {
  Serial.begin(9600);
  if (wst.needSetup()) {
    wst.prepareServing();
    Serial.println("serving");
  } else {
    wst.tryConnectingWiFi();  
  }
  
}

void loop() {
  if (wst.needSetup()) {
    delay(400);
    wst.serveWeb();
    if (wst.needRestart()) {
      Serial.println("Restart in 3 sec");
      delay(3000);
      ESP.restart();
    }
  }
}
