#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

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

void wifi_connect(LiquidCrystal_I2C lcd) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("...");
  }
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());

  __client.setInsecure();
}

WiFiClientSecure wifi_client() {
  return __client;
}
