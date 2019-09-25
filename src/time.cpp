#include <TimeLib.h>

#include "time.h"
#include "http.h"

const String TIME_URL = "https://worldtimeapi.org/api/timezone/Europe/Berlin.txt";

// Forward declarations
void __print_time();
String __extract_response_field(String fieldName, String data);

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
        String data = response->body;
        String epochSeconds = __extract_response_field("unixtime", data);
        if (epochSeconds.length() == 10)
        {
            time_t epochTime = epochSeconds.toInt();
            
            String utcOffset = __extract_response_field("utc_offset", data);
            boolean shouldAdd = utcOffset.charAt(0) == '+';
            uint8 utcHoursOffset = utcOffset.substring(1, 3).toInt();
            uint8 utcMinutesOffset = utcOffset.substring(4).toInt();
            int totalOffset = 60 * (60 * utcHoursOffset + utcMinutesOffset);

            if (shouldAdd) {
                epochTime += totalOffset;
            } else {
                epochTime -= totalOffset;
            }

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
            __time.epoch = -1;
            Serial.println("[TIME] Expected 10 characters repsonse, got body: " + data);
        }
    }
    else
    {
        __time.epoch = -1;
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

    Serial.println("[TIME] Current time: " + date + " - " + time);
}

String __extract_response_field(String fieldName, String data)
{
    int lineStart = data.indexOf(fieldName);
    int beginTimestamp = lineStart + fieldName.length() + 2;
    int endTimestamp = data.indexOf('\n', beginTimestamp);
    return data.substring(beginTimestamp, endTimestamp);
}
