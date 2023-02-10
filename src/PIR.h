#include <Arduino.h>
#include "Define.h"

bool PIR_on = true;

bool pir = false; // true -> pet moved, false -> pet not moved

extern time_t timestamp;

void PIR(void *param) {
    POST_pir();
    bool last = pir;
    while (1) {
        if (!PIR_on)
            continue;
        pir = digitalRead(PIR_pin);
        if (last == false && pir == true) {
            Serial.print("pet moved at time ");
            Serial.println(timestamp);
            POST_pir();
        }
        last = pir;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
