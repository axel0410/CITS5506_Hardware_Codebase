#ifndef ESP_WIFI_h
#define ESP_WIFI_h

#include "Arduino.h"
#include <WiFi.h>

class ESP_WIFI{
  public:
    //scan for wifi networks in the area
    void scanWIFI();
    //connect to wifi network via ssid and password
    void connectWIFI(const char* ssid, const char* password);
    //reconnect to wifi if wifi disconnects 
    void reconnectWIFI();
    //disconnect from wifi
    void disconnectWIFI();
    //returns true if wifi is connected
    bool isConnected();
    //gets the devices mac address
    String getMacAddress();

};

#endif
