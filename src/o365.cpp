#include <EEPROM.h>

#include "o365.h"

O365CalendarEvent __o365CalendarEvent;

// Static OAuth Configuration Data
struct OAuthData {
  char tenantId[37];
  char clientId[37];
  char clientSecret[33];
  char resourceId[37];
} o365_auth;

// Base URL for requests
String graphBaseUrl = "https://graph.microsoft.com/v1.0/";

// OAuth token to use - populated by refresh
String token;

// Workday times - HH:mm
String workDayStart = "08:00";
String workDayEnd = "20:00";
String maxEvents = "2";


// Static computation for /events JSON Body
size_t eventsBodyCapacity = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument eventsBodyDoc(eventsBodyCapacity);

void __read_from_serial(String prompt, char* ptr, uint8_t len) {
  Serial.println("[O365] " + prompt + ":");
  String data;
  data = Serial.readStringUntil('\n');
  data.toCharArray(ptr, len + 1);
}

void __print_auth_configuration() {
  Serial.println();
  Serial.println("[O365] auth.tenantId: " + String(o365_auth.tenantId));
  Serial.println("[O365] auth.clientId: " + String(o365_auth.clientId));
  Serial.println("[O365] auth.clientSecret: " + String(o365_auth.clientSecret));
  Serial.println("[O365] auth.resourceId: " + String(o365_auth.resourceId));
  Serial.println();
}

void __read_auth_configuration() {
  __read_from_serial("Enter TENANT", o365_auth.tenantId, 36);
  __read_from_serial("Enter CLIENT ID", o365_auth.clientId, 36);
  __read_from_serial("Enter CLIENT SECRET", o365_auth.clientSecret, 32);
  __read_from_serial("Enter RESOURCE ID", o365_auth.resourceId, 36);
}

String __split_value(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void o365_init() {
  EEPROM.begin(sizeof(o365_auth));
  EEPROM.get(0, o365_auth);

  Serial.println("[O365] Initialized from memory...");
  __print_auth_configuration();

  String data;
  Serial.setTimeout(10000);
  Serial.println("[O365] Setup - press any character and Enter to setup (10s)...");
  data = Serial.readStringUntil('\n');
  if (data == "") {
    Serial.println("[O365] Skipped setup...");
  } else {
    __read_auth_configuration();
    __print_auth_configuration();
    
    EEPROM.put(0, o365_auth);
    Serial.println("[O365] Wrote settings to EEPROM...");
  }
  Serial.setTimeout(1000);
  
  EEPROM.end();
}

void o365_refresh_token() {
  String tenant = String(o365_auth.tenantId);
  String tokenUrl = "https://login.microsoftonline.com/" + tenant + "/oauth2/v2.0/token";
  
  String payload = "client_id=" + String(o365_auth.clientId) +
                    "&scope=https%3A%2F%2Fgraph.microsoft.com%2F.default" +
                    "&client_secret=" + String(o365_auth.clientSecret) +
                    "&grant_type=client_credentials";
                    
  HttpResponse* response;
  response = http_post_form(tokenUrl, payload);

  if (response->code == 200) {
    size_t capacity = JSON_OBJECT_SIZE(4) + 1398 + 50;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, response->body);
    token = doc["access_token"].as<String>();
    Serial.println("[O365] Got token: " + token);
  } else {
    Serial.println("[O365] Failed to accquire token...");
  }
}

O365CalendarEvent* o365_get_events(String currentDate) {
  String headers[3][2] = {
    { "Authorization", "Bearer " + String(token) },
    { "Content-Type", "application/json" },
    { "Prefer", "outlook.timezone=\"Europe/Berlin\"" }
  };

  String baseUrl = graphBaseUrl + "users/" + String(o365_auth.resourceId) + "/calendar/events";
  
  // Query-parameters: https://docs.microsoft.com/en-us/graph/query-parameters
  String query = "$select=start,end,organizer" + 
                  String("&$orderby=start/dateTime%20asc") + 
                  "&$top=" + maxEvents +
                  "&$filter=start/dateTime%20ge%20'" + currentDate + "T" + workDayStart + "'%20and%20end/dateTime%20lt%20'" + currentDate + "T"+ workDayEnd + "'";

  String APIUrl = baseUrl + String("?") + query;
  
  HttpResponse* response;
  response = http_get(APIUrl, headers, 3);

  if (response->code == 200) {
    const size_t capacity = JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(1) + 6*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + 1390 + 50;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, response->body);
    JsonArray meetings = doc["value"];
    uint8_t len = meetings.size();
    Serial.println("[O365] Found " + String(len) + " meetings today");
    if (len > 0 ) {
      JsonObject meeting_0 = doc["value"][0];
      __o365CalendarEvent.startTime = __split_value(meeting_0["start"]["dateTime"].as<String>(), 'T', 1);
      __o365CalendarEvent.endTime = __split_value(meeting_0["end"]["dateTime"].as<String>(), 'T', 1);
      __o365CalendarEvent.organiser = meeting_0["organizer"]["emailAddress"]["name"].as<String>();
      Serial.println("[O365] First meeting " + String(__o365CalendarEvent.startTime) + " to " + String(__o365CalendarEvent.endTime) + " organised by " + String(__o365CalendarEvent.organiser));
    }
  } else {
    Serial.println("[O365] Failed to accquire meeting update...");
  }
  return &__o365CalendarEvent;
}
