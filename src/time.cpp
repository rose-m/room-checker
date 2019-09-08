#include <TimeLib.h>

#include "time.h"
#include "http.h"

const String TIME_URL = "https://currentmillis.com/time/minutes-since-unix-epoch.php";

// Forward declarations
void __print_time();

Time __time;

Time time_get()
{
    return __time;
}

Time time_update()
{
    Serial.println("[TIME] Updating time...");

    HttpResponse *response = http_get(TIME_URL);
    if (response->code == 200)
    {
        String epochMinutes = response->body;
        if (epochMinutes.length() == 8)
        {
            time_t epochTime = epochMinutes.toInt() * 60;
            setTime(epochTime);
            __time.year = year();
            __time.month = month();
            __time.day = day();
            __time.hour = hour();
            __time.minute = minute();
            __time.epoch = epochTime;
            __print_time();
        }
        else
        {
            Serial.println("[TIME] Expected 8 characters repsonse, got body: " + epochMinutes);
        }
    }
    else
    {
        Serial.println("[TIME] Failed to update time, HTTP response code: " + String(response->code));
    }

    return __time;
}

void __print_time()
{
    char data[10];
    sprintf(data, "%04d-%02d-%02d", __time.year, __time.month, __time.day);
    String date = String(data);

    sprintf(data, "%02d:%02d", __time.hour, __time.minute);
    String time = String(__time.hour) + ":" + String(__time.minute);

    Serial.println("[TIME] Current time: " + year());
}
