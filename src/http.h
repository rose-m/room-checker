#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClientSecure.h>

#ifndef HTTP_H_
#define HTTP_H_

struct HttpResponse {
  int code;
  String body;
};

HttpResponse* http_get(String url, String headers[][2] = NULL, uint8_t len = 0);
HttpResponse* http_post(String url, String body, String headers[][2] = NULL, uint8_t len = 0);
HttpResponse* http_post_form(String url, String formData);

#endif
