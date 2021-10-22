#include "ESP_Buzzer.h"

ESP_Buzzer::ESP_Buzzer(int buzzpin){
  m_bpin = buzzpin;
  ledcSetup(0,1E5,12);
  ledcAttachPin(m_bpin,0);

  m_loop = false;
}

void ESP_Buzzer::startBuzzLoop(int btone, int bdelay){
  m_loop = true;
  //Serial.println("Starting buzz looP");
  while(m_loop){
    ledcWriteTone(0,btone);
    delay(bdelay);
    ledcWrite(0,0);
    delay(bdelay);
  }
  vTaskDelete(NULL);
}

void ESP_Buzzer::endBuzzLoop(){
  //Serial.println("Ending buzz looP");
  m_loop = false;
}

bool ESP_Buzzer::isBuzzing(){
  return m_loop;
}
