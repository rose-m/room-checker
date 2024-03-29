#include <ArduinoJson.h>

#include "config.h"
#include "http.h"

#ifndef O365_H_
#define O365_H_

struct O365CalendarEvent {
  String organiser;
  String startTime;
  String endTime;
};

void o365_init();

TokenData* o365_refresh_token();

O365CalendarEvent* o365_get_events();

#endif
