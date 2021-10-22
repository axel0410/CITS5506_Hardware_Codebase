#ifndef ESP_HTTP_h
#define ESP_HTTP_h

#include "Arduino.h"
#include <HTTPClient.h>
#include <WiFi.h>

class ESP_HTTP{
  public:
    //send JSON data to a server using post
    static int sendJSON(String server, String payload);
    //get JSON data from a server using get
    static String getJSON(String server);
};

#endif
