#include "src/http.h"
#include "src/o365.h"
#include "src/oled.h"
#include "src/wifi.h"

/* ==================== */
/* Program variables */
const int intervalMs = 60 * 1000;
/* -- Program variables */
/* ==================== */

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.println("Initializing OLED...");
  oled_init();
  Serial.println("Initialized...");

  oled_print_top("Init WIFI");
  wifi_connect();
  delay(2000);
  oled_clear();

  o365_init();
  o365_refresh_token();
}

void loop()
{
  oled_clear();

  String currentDate = "2019-08-16";
  O365CalendarEvent *event = o365_get_events(currentDate);
  if (event == NULL)
  {
    oled_print_top("FREE");
    oled_print_bottom("No events...");
  }
  else if (isBooked(event))
  {
    // Red LED ON
    // Green LED OFF
    oled_print_top("Booked until:");
    oled_print_bottom(String(event->endTime).substring(0, 5));
  }
  else
  {
    // Red LED OFF
    // Green LED ON
    oled_print_top("Next event:");
    oled_print_bottom(String(event->startTime).substring(0, 5) + " - " + String(event->endTime).substring(0, 5));
  }

  delay(intervalMs);
}

boolean isBooked(O365CalendarEvent *event)
{
  return false;
}
