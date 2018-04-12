#include <WiFiSetup.h>

WiFiSetup wst(13, "HelloG");

void setup() {
  wst.prepareServing();
  Serial.println("serving");
}

void loop() {
  wst.serveWeb();
}
