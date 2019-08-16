#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClientSecure.h>

#ifndef HTTP_H_
#define HTTP_H_

struct HttpResponse {
  int code;
  String body;
};

#endif
