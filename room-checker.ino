#include "http.h"
#include "o365.h"
#include "lcd.h"

/* ==================== */
/* Program variables */
const int intervalMs = 60 * 1000;
/* -- Program variables */
/* ==================== */

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.println("Initializing LCD...");
  lcd_init();
  Serial.println("Initialized...");

  lcd_print_top("Init WIFI");
  wifi_connect();
  delay(2000);
  lcd_clear();

  o365_init();
  o365_refresh_token();
}

void loop() {
  lcd_clear();
  
  String currentDate = "2019-08-16";
  O365CalendarEvent* event = o365_get_events(currentDate);
  if (isBooked(event)) {
    // Red LED ON
    // Green LED OFF
    lcd_print_top("Booked until:");
    lcd_print_bottom(String(event->endTime).substring(0,5));
  } else {
    // Red LED OFF
    // Green LED ON
    lcd_print_top("Booked until:");
    lcd_print_bottom(String(event->startTime).substring(0,5) + " - " + String(event->endTime).substring(0,5));
  }

  delay(intervalMs);
}

boolean isBooked(O365CalendarEvent* event) {
  return false;
}
