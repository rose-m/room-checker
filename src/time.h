#include <Arduino.h>

#ifndef TIME_H_
#define TIME_H_

struct Time {
    uint8 day;
    uint8 month;
    uint8 year;
    uint8 hour;
    uint8 minute;
    time_t epoch;
};

Time time_get();

Time time_update();

#endif