#include <WiFi.h>

const char *ssid = "base_ID"; // ID da base de carregamento

void setup() {
  //Indica base ligada
  pinMode(LED_BUILTIN, OUTPUT);

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid);
}

void loop() {}
