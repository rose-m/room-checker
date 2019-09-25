#include "o365.h"
#include "time.h"

O365CalendarEvent __o365CalendarEvent;

// Base URL for requests
String graphBaseUrl = "https://graph.microsoft.com/v1.0/";

// Workday times - HH:mm
String workDayStart = "08:00";
String workDayEnd = "20:00";
String maxEvents = "2";

// Static computation for /events JSON Body
size_t eventsBodyCapacity = JSON_ARRAY_SIZE(1) + 2 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument eventsBodyDoc(eventsBodyCapacity);

String __split_value(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void o365_init()
{
  Serial.println("[O365] Initializing...");
}

TokenData* o365_refresh_token()
{
  String tenant = config_get_option(TENANT_ID);
  String tokenUrl = "https://login.microsoftonline.com/" + tenant + "/oauth2/v2.0/token";

  String payload = "client_id=" + config_get_option(CLIENT_ID) +
                   "&scope=https%3A%2F%2Fgraph.microsoft.com%2F.default" +
                   "&client_secret=" + config_get_option(CLIENT_SECRET) +
                   "&grant_type=client_credentials";

  HttpResponse *response;
  response = http_post_form(tokenUrl, payload);

  if (response->code == 200)
  {
    size_t capacity = JSON_OBJECT_SIZE(4) + 1500 + 50;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, response->body);

    String token = doc["access_token"].as<String>();
    int expiresIn = doc["expires_in"].as<int>();
    Serial.println("[O365] Got token: " + token + ", expires in: " + String(expiresIn));

    TokenData *tokenData = new TokenData();
    tokenData->token = token;
    tokenData->validUntilEpoch = time_get().epoch + expiresIn - 30; // we subtract 30 seconds for good measure
    return tokenData;
  }
  else
  {
    Serial.println("[O365] Failed to accquire token...");
    return NULL;
  }
}

O365CalendarEvent *o365_get_events(String currentDate)
{
  String headers[3][2] = {
      {"Authorization", "Bearer " + config_get_option(TOKEN)},
      {"Content-Type", "application/json"},
      {"Prefer", "outlook.timezone=\"Europe/Berlin\""}};

  String baseUrl = graphBaseUrl + "users/" + config_get_option(RESOURCE_ID) + "/calendar/events";

  // Query-parameters: https://docs.microsoft.com/en-us/graph/query-parameters
  String query = "$select=start,end,organizer" +
                 String("&$orderby=start/dateTime%20asc") +
                 "&$top=" + maxEvents +
                 "&$filter=start/dateTime%20ge%20'" + currentDate + "T" + workDayStart + "'%20and%20end/dateTime%20lt%20'" + currentDate + "T" + workDayEnd + "'";

  String APIUrl = baseUrl + String("?") + query;

  HttpResponse *response;
  response = http_get(APIUrl, headers, 3);

  if (response->code == 200)
  {
    const size_t capacity = JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(1) + 6 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2 * JSON_OBJECT_SIZE(5) + 1390 + 50;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, response->body);
    JsonArray meetings = doc["value"];
    uint8_t len = meetings.size();
    Serial.println("[O365] Found " + String(len) + " meetings today");
    if (len > 0)
    {
      JsonObject meeting_0 = doc["value"][0];
      __o365CalendarEvent.startTime = __split_value(meeting_0["start"]["dateTime"].as<String>(), 'T', 1);
      __o365CalendarEvent.endTime = __split_value(meeting_0["end"]["dateTime"].as<String>(), 'T', 1);
      __o365CalendarEvent.organiser = meeting_0["organizer"]["emailAddress"]["name"].as<String>();
      Serial.println("[O365] First meeting " + String(__o365CalendarEvent.startTime) + " to " + String(__o365CalendarEvent.endTime) + " organised by " + String(__o365CalendarEvent.organiser));
    }
  }
  else
  {
    Serial.println("[O365] Failed to accquire meeting update...");
  }
  return &__o365CalendarEvent;
}
