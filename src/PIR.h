/*
    POST
*/
#include <Arduino.h>
#include "Define.h"

// bool pet_active; //True -> pet in room, False -> pet not in room
bool PIR_on = true; //True-> on, False -> off
extern time_t timestamp;

void PIR(void *param) {
  bool last = false;
  bool current;
  while (1) {
    if (!PIR_on)
        continue;
    current = digitalRead(PIR_pin);
    if (last == false && current == true) {
      Serial.print("cat moved at time ");
      Serial.println(timestamp);
      //POST_pir();
    }
    last = current;
    vTaskDelay(300/portTICK_PERIOD_MS);
  }
}