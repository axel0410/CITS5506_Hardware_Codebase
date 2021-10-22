#include "ESP_HTTP.h"

int ESP_HTTP::sendJSON(String server, String payload){
  HTTPClient http;
  WiFiClient wifi;
  
  http.begin(wifi, server);
  http.setTimeout(10000);

  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(payload);
  
  //Serial.print("HTTP Response code POST: ");
  //Serial.println(httpResponseCode);
  
  http.end();

  return httpResponseCode;
}

String ESP_HTTP::getJSON(String server){
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(server);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 

  if (httpResponseCode>0) {
    //Serial.print("HTTP Response code GET: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
