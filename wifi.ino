#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "lcd.h"

/* ==================== */
/* WIFI Constants */
#ifndef STASSID
#define STASSID "CF Guests"
#define STAPSK  "CFwelcomesGuests"
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
    lcd_print_bottom("...");
  }
  lcd_print_bottom(WiFi.localIP().toString());

  __client.setInsecure();
}

WiFiClientSecure wifi_client() {
  return __client;
}
