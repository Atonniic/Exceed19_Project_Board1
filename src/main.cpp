#include <Arduino.h>
#include "Define.h"
#include "Tank.h"
#include "PIR.h"
#include "HTTP.h"

//time
time_t timestamp;

//ntp
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

void setup() {
    Serial.begin(115200);

    pinMode(PIR_pin, INPUT);
    pinMode(RED_pin, OUTPUT);
    pinMode(GREEN_pin, OUTPUT);
    pinMode(Laser_pin, OUTPUT);
    pinMode(LDR_pin, INPUT);

    digitalWrite(Laser_pin, HIGH);

    connectWifi();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    xTaskCreatePinnedToCore(PIR, "PIR", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(Tank, "Tank", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(GET_commands, "GET_commands", 10000, NULL, 1, NULL, 1)
}

void loop() {
    time(&timestamp);
}