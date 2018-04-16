#include <WiFiSetup.h>

WiFiSetup wst(12, "HelloGo");

void setup() {
  Serial.begin(9600);
  if (wst.autoSetup()) {
    return;
  }
}

void loop() {
  if (wst.autoLoop()) {
    return;
  }
}
