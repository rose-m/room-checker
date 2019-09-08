#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#ifndef WIFI_H_
#define WIFI_H_

void wifi_connect();

WiFiClientSecure wifi_client();

#endif