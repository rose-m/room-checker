#include <LiquidCrystal_I2C.h>

#include "http.h"

/* ==================== */
/* LCD Constants */
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
/* -- LCD Constants */
/* ==================== */


/* ==================== */
/* Program variables */
const int intervalMs = 10 * 1000;

//const char* host = "base.cplace.io";
const char* url = "https://base.cplace.io/applicationStatus.txt";
const char* expectedResult = "OK";
const int httpsPort = 443;
/* -- Program variables */
/* ==================== */

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.println("Initializing LCD...");
  lcd.init();
  Serial.println("Initialized...");
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Init WIFI");
  wifi_connect(lcd);
  delay(2000);
  lcd.clear();

  o365_init();
  o365_refresh_token();
}

void loop() {
  lcd.clear();
  if (isHealthy()) {
    lcd.setCursor(0, 1);
    lcd.print("Result: OK");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Result: DOWN!!!");
  }
  delay(intervalMs);
}

boolean isHealthy() {
  lcd.setCursor(0, 0);
  lcd.print("Checking...");

//  WiFiClientSecure client = wifi_client();
//
//  if (!client.connect(host, httpsPort)) {
//    lcd.setCursor(0, 0);
//    lcd.print("Connection failed!");
//    return false;
//  }
//
//  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" + 
//               "User-Agent: TheMakerLab\r\n" +
//               "Connection: close\r\n\r\n");
//
//  while (client.connected()) {
//    String line = client.readStringUntil('\n');
//    if (line == "\r") {
//      break;
//    }
//  }
//
//  String line = client.readStringUntil('\n');

  HttpResponse* response;
  response = http_get(url);

  lcd.setCursor(0, 0);
  lcd.print("Check completed.");
  
  return response->code == 200 && response->body == String("OK");
}
