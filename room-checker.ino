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
const int INTERVAL_MS = 10 * 1000;
const uint64 SLEEP_AT_NIGHT = 60 * 60e6; // 60 Minutes
const uint64 SLEEP_AT_DAY = 10e6;        // 10 seconds
//const uint64 SLEEP_AT_DAY = 5 * 60e6;    // 5 Minutes

const uint8 PIN_LED_RED = D8;
const uint8 PIN_LED_GREEN = D7;
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

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);

  config_init();
  oled_init();
  wifi_connect();

  delay(2000);
  oled_clear();

  Time time;
  do
  {
    time = time_update();
    delay(5000);
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

  //Serial.println("[MAIN] Time to sleep...");
  //delay(10000);
  //return;
  //ESP.deepSleep(SLEEP_AT_DAY);

  // TODO: CONTINUE.......

  O365CalendarEvent *event = o365_get_events();
  if (event == NULL)
  {
    // Red LED OFF
    digitalWrite(PIN_LED_RED, LOW);
    // Green LED ON
    digitalWrite(PIN_LED_GREEN, HIGH);

    oled_print_top("FREE");
    oled_print_bottom("No bookings...");
  }
  else
  {
    if (__is_booked(event))
    {
      // Red LED ON
      digitalWrite(PIN_LED_RED, HIGH);
      // Green LED OFF
      digitalWrite(PIN_LED_GREEN, LOW);

      oled_print_top("Booked until:");
      oled_print_bottom(String(event->endTime));
    }
    else
    {
      // Red LED OFF
      digitalWrite(PIN_LED_RED, LOW);
      // Green LED ON
      digitalWrite(PIN_LED_GREEN, HIGH);

      oled_print_top("Next booking:");
      oled_print_bottom(String(event->startTime) + " - " + String(event->endTime));
    }

    delete event;
  }

  delay(INTERVAL_MS);
}

boolean __is_booked(O365CalendarEvent *event)
{
  int startHour = event->startTime.substring(0, 2).toInt();
  int startMinute = event->startTime.substring(3, 5).toInt();
  Time t = time_get();
  return startHour < t.hour || (startHour == t.hour && startMinute <= t.minute);
}

boolean __should_sleep_at_night(Time time)
{
  return time.hour > 20 || time.hour < 6;
}