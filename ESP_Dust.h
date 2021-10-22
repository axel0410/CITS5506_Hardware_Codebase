#ifndef ESP_Dust_h
#define ESP_Dust_h

#include "Arduino.h"

class ESP_Dust{
  public:
    ESP_Dust(int ledPin, int dataPin);
    //updates data values 
    void pollSensor();
    //getters for sensor data
    double getDustDensity();
    double getVoltage();

  private:
    const int m_samplingTime = 280;
    const int m_deltaTime = 40;
    const int m_sleepTime = 9680;

    int m_ledPin;
    int m_dataPin;

    double m_voltage;
    double m_dustDensity;

    double valmod(double vol);
    
};

#endif
