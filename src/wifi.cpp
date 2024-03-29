#include "wifi.h"
#include "oled.h"

/* ==================== */
/* WIFI Constants */
#ifndef STASSID
#define STASSID "CF Guest"
#define STAPSK  "CFwelcomesGuests"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
/* -- WIFI Constants */
/* ==================== */

WiFiClientSecure __client;

void wifi_connect() {
  oled_print_top("Init WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    oled_print_bottom("...");
  }

  String ip = WiFi.localIP().toString();
  Serial.println("[WIFI] IP: " + ip);
  oled_print_bottom(ip);
  delay(2000);

  __client.setInsecure();
}

WiFiClientSecure wifi_client() {
  return __client;
}
