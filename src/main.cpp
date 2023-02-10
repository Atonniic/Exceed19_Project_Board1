#include <Arduino.h>
#include "Define.h"
#include "Tank.h"
#include "PIR.h"
#include "HTTP.h"
#include <time.h>

const int room_id = 0;

const char *ssid = "Pun-iPhone";
const char *password = "spiderman";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

time_t timestamp;

void setup() {
    Serial.begin(115200);

    pinMode(PIR_pin, INPUT);
    pinMode(RED_pin, OUTPUT);
    pinMode(GREEN_pin, OUTPUT);
    pinMode(Laser_pin, OUTPUT);
    pinMode(LDR_pin, INPUT);

    connectWifi();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    while ((timestamp = time(NULL)) < 120) {
        Serial.println("Waiting for NTP time sync: " + String(timestamp));
        delay(1000);
    }

    xTaskCreatePinnedToCore(PIR, "PIR", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(Tank, "Tank", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(GET_commands, "GET_commands", 10000, NULL, 1, NULL, 1)
}

void loop() {
    time(&timestamp);
}
