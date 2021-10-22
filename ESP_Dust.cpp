#include "ESP_Dust.h"

ESP_Dust::ESP_Dust(int ledPin, int dataPin){
  m_ledPin = ledPin;
  m_dataPin = dataPin;
  pinMode(m_ledPin, OUTPUT);
  
  m_dustDensity = 0.0;
  m_voltage = 0.0;
}

void ESP_Dust::pollSensor(){
  digitalWrite(m_ledPin, LOW); // power on the LED
  delayMicroseconds(m_samplingTime);
  
  int voMeasured = analogRead(m_dataPin); // read the dust value
  
  delayMicroseconds(m_deltaTime);
  digitalWrite(m_ledPin, HIGH); // turn the LED off
  delayMicroseconds(m_sleepTime);
  //calculate dust value
  m_voltage = (voMeasured * (3.3 / 4095)) * 1.33;
  m_voltage = valmod(m_voltage);
  m_dustDensity = (0.17 * m_voltage - 0.1);

  if (m_dustDensity < 0) {
    m_dustDensity = 0.00;
  }
}

double ESP_Dust::valmod(double vol){
  double nv = vol;
  if(vol < 0.9){
    nv = vol * 1.5;
  }
  
  if(vol < 1.8 && vol > 0.9){
    nv = vol * 1.2;
  }

  return nv;
}

double ESP_Dust::getDustDensity(){
  return m_dustDensity;
}

double ESP_Dust::getVoltage(){
  return m_voltage;
}
