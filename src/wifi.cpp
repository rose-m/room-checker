#include "wifi.h"
#include "oled.h"

/* ==================== */
/* WIFI Constants */
#ifndef STASSID
#define STASSID "MakerLab"
#define STAPSK  "2889130479059627"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
/* -- WIFI Constants */
/* ==================== */

WiFiClientSecure __client;

void wifi_connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    oled_print_bottom("...");
  }
  oled_print_bottom(WiFi.localIP().toString());

  __client.setInsecure();
}

WiFiClientSecure wifi_client() {
  return __client;
}
