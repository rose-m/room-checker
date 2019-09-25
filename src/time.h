#include <Arduino.h>

#ifndef TIME_H_
#define TIME_H_

struct Time {
    uint8 day;
    uint8 month;
    uint16 year;
    uint8 hour;
    uint8 minute;
    time_t epoch;
};

Time time_get();

Time time_update();

String time_to_datetime_string();

String time_to_time_string();

String time_to_date_string();

#endif