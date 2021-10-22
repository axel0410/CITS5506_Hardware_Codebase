#ifndef ESP_Buzzer_h
#define ESP_Buzzer_h

#include "Arduino.h"

class ESP_Buzzer{
  public:
    ESP_Buzzer(int buzzpin);
    //Loops the buzzer until it is set to false
    void startBuzzLoop(int btone, int bdelay);
    //use to stop buzzer loop
    void endBuzzLoop();
    //returns true if in the buzzer loop
    bool isBuzzing();
    

  private:
    int m_bpin;
    bool m_loop;

};

#endif
