#include "src/config.h"
#include "src/http.h"
#include "src/time.h"
#include "src/o365.h"
#include "src/oled.h"
#include "src/wifi.h"

/* ==================== */
/* Forward declarations */
/* ==================== */
boolean __should_sleep_at_night();
/* ==================== */
/* -- Forward declarations */
/* ==================== */

/* ==================== */
/* Program variables */
const int INTERVAL_MS = 60 * 1000;
const uint64 SLEEP_AT_NIGHT = 60 * 60e6; // 60 Minutes
const uint64 SLEEP_AT_DAY = 10e6;    // 10 seconds
//const uint64 SLEEP_AT_DAY = 5 * 60e6;    // 5 Minutes
/* -- Program variables */
/* ==================== */

void setup()
{
  // General power-saving flow:
  // 1. Check for Setup needs
  // 2. Initialize OLED
  // 3. Initialize WiFi
  // 4. Update Time?
  //  4a. If at night, sleep again
  // 5. Check Token validity
  //  5a. Update Token
  // 6. Check for next meetings
  // 7. Update Display

  Serial.begin(115200);
  while (!Serial)
  {
    // Wait for initialization
  }

  config_init();
  oled_init();
  wifi_connect();

  delay(2000);
  oled_clear();

  Time time;
  do {
    time = time_update();
    delay(1000);
  } while (time.epoch < 0);

  if (__should_sleep_at_night(time))
  {
    // nothing to happen anymore
    Serial.println("[MAIN] Shhhh, it's night time...");
    //ESP.deepSleep(SLEEP_AT_NIGHT);
    //return;
  }

  // We now know o365 work is required - let the loop do the rest
  o365_init();
}

void loop()
{
  TokenData tokenData = config_get_token();
  if (tokenData.validUntilEpoch < time_get().epoch || tokenData.token.length() == 0)
  {
    TokenData *newTokenData = o365_refresh_token();
    config_update_token(newTokenData);
    delete newTokenData;
  }

  oled_print_top("Hello");
  oled_print_bottom("Vincent!");

  Serial.println("[MAIN] Time to sleep...");
  delay(10000);
  return;
  //ESP.deepSleep(SLEEP_AT_DAY);

  // TODO: CONTINUE.......

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

  delay(INTERVAL_MS);
}

boolean isBooked(O365CalendarEvent *event)
{
  return false;
}

boolean __should_sleep_at_night(Time time)
{
  return time.hour > 20 || time.hour < 6;
}