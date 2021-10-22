#include "ESP_DHT.h"

ESP_DHT::ESP_DHT(int pin){
  m_pin = pin;
  m_dht = new DHT(m_pin, DHT22);
  m_dht->begin();
  m_temp = 0.0;
  m_hum = 0.0;
}

void ESP_DHT::pollSensor(){
  double tempt,temph;
  
  tempt = m_dht->readTemperature();
  temph = m_dht->readHumidity();
  
  if(!isnan(tempt))
    m_temp = tempt;
  if(!isnan(temph))
    m_hum = temph;

}

double ESP_DHT::getTemperature(){
  return m_temp;
}

double ESP_DHT::getHumidity(){
  return m_hum;
}

double ESP_DHT::getHeatIndex(){
  return m_dht->computeHeatIndex(m_temp, m_hum, false);
}
