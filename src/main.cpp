#include <Arduino.h>
#include "Define.h"
#include "Tank.h"
#include "PIR.h"
#include "HTTP.h"

bool tank_level; //0 -> nothing, 1 -> refill
bool pet_active; //True -> pet in room, False -> pet not in room
bool PIR_on; //True-> on, False -> off

//server
int room_id = 0;
int count = 0;
int timestamp;

void setup() {
    Serial.begin(115200);
    pinMode(PIR_pin, INPUT);
    pinMode(RED_pin, OUTPUT);
    pinMode(GREEN_pin, OUTPUT);
    pinMode(Laser_pin, OUTPUT);
    digitalWrite(Laser_pin, HIGH);
    //connectWifi();

    //xTaskCreatePinnedToCore(PIR, "PIR", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(Tank, "Tank", 10000, NULL, 1, NULL, 0);
    //xTaskCreatePinnedToCore(HTTP, "HTTP", 10000, NULL, 1, NULL, 1);
}

void loop() {
}