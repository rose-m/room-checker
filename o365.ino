#include <EEPROM.h>
#include <ArduinoJson.h>

#include "http.h"

// Static OAuth Configuration Data
struct OAuthData {
  char tenantId[37];
  char clientId[37];
  char clientSecret[33];
  char resourceId[37];
} o365_auth;

// Base URL for requests
String graphBaseUrl = "https://graph.microsoftonline.com";

// OAuth token to use - populated by refresh
String token;

// Static computation for /events JSON Body
size_t eventsBodyCapacity = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 200;
StaticJsonDocument eventsBodyDoc(eventsBodyCapacity);

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

void o365_get_events() {
  String headers[3][2] = {
    { "Authorization", "Bearer " + token },
    { "Content-Type", "application/json" },
    { "Prefer", "outlook.timezone='Europe/Berlin'" }
  };

  

  deserializeJson(doc, response->body);
  token = doc["access_token"].as<String>();
  Serial.println("[O365] Got token: " + token);
  
//  http_post();
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

void __read_from_serial(String prompt, char* ptr, uint8_t len) {
  Serial.println("[O365] " + prompt + ":");
  String data;
  data = Serial.readStringUntil('\n');
  data.toCharArray(ptr, len + 1);
}
