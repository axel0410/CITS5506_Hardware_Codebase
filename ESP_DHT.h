#ifndef ESP_DHT_h
#define ESP_DHT_h

#include "Arduino.h"

#include <DHT.h>

class ESP_DHT{
  public:
    ESP_DHT(int pin);
    //polls the sensor to update values 
    void pollSensor();
    //getters to get sensor values, only updated if pollSensor is called
    double getTemperature();
    double getHumidity();
    double getHeatIndex();

  private:
    double m_temp;
    double m_hum;
    DHT* m_dht;
    int m_pin;
    
};

#endif
