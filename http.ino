
HttpResponse __response;

HttpResponse* http_get(String url, String headers[][2], uint8_t len) {
  WiFiClientSecure client = wifi_client();
  HTTPClient https;

  Serial.println("[HTTP] GET " + url + "...");
  if (https.begin(client, url)) {
    for (uint8_t i = 0; i < len; i++) {
      String *h = headers[i];
      https.addHeader(h[0], h[1]);
    }
    __response.code = https.GET();
    Serial.println("[HTTP] GET " + url + " - " + __response.code);

    if (__response.code > 0) {
      __response.body = https.getString();
    }
    
    https.end();
  } else {
    Serial.println("[HTTP] Unable to connect");
    __response.code = -999;
  }
  
  return &__response;
}

HttpResponse* http_post(String url, String body, String headers[][2], uint8_t len) {
  WiFiClientSecure client = wifi_client();
  HTTPClient https;

  Serial.println("[HTTP] POST " + url + "...");
  if (https.begin(client, url)) {
    for (uint8_t i = 0; i < len; i++) {
      String *h = headers[i];
      https.addHeader(h[0], h[1]);
    }
    
    __response.code = https.POST(body);
    Serial.println("[HTTP] POST " + url + " - " + __response.code);
    if (__response.code > 0) {
      __response.body = https.getString();
    }
    
    https.end();
  } else {
    Serial.println("[HTTP] Unable to connect");
    __response.code = -999;
  }
  
  return &__response;
}

HttpResponse* http_post_form(String url, String formData) {
  String headers[1][2] = {
    { "Content-Type", "application/x-www-form-urlencoded" }
  };
  return http_post(url, formData, headers, 1);
}
