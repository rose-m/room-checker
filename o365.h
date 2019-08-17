#include <EEPROM.h>
#include <ArduinoJson.h>

#include "http.h"

#ifndef O365_H_
#define O365_H_

struct O365CalendarEvent {
  String organiser;
  String startTime;
  String endTime;
};

#endif
