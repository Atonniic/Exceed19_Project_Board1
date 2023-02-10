#include <Arduino.h>
#include "Define.h"
#include "HTTP.h"

bool tank_level = false; // true -> refill needed, false -> refill not needed

void Tank(void *param) {
    digitalWrite(Laser_pin, HIGH);
    PUT_tank_level();
    bool last = tank_level;
    while (1) {
        tank_level = analogRead(LDR_pin) >= 2500;
        if (last != tank_level) {
            digitalWrite(RED_pin, tank_level);
            digitalWrite(GREEN_pin, !tank_level);
            PUT_tank_level();
        }
        last = tank_level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
