// firmware for board 1 (LED, Tank sensors, PIR)

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <TridentTD_LineNotify.h>

#define RED_pin 25
#define GREEN_pin 26
#define PIR_pin 27
#define Laser_pin 33
#define LDR_pin 34

#define LDR_threshold 1800

#define LINE_TOKEN "";

const int room_id = 0;

const char *ssid = "";
const char *password = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

const String baseUrl = "https://ecourse.cpe.ku.ac.th/exceed12/";

time_t timestamp;
int count = 0;
bool tank_level = false; // true -> refill needed, false -> refill not needed
bool pir = false; // true -> pet moved, false -> pet not moved
bool PIR_on = true;

void connectWifi();
void Tank(void *param);
void PIR(void *param);
void PUT_tank_level();
void POST_pir();
void GET_pir_command(void *param);

void setup() {
    Serial.begin(115200);

    pinMode(RED_pin, OUTPUT);
    pinMode(GREEN_pin, OUTPUT);
    pinMode(PIR_pin, INPUT);
    pinMode(Laser_pin, OUTPUT);
    pinMode(LDR_pin, INPUT);

    digitalWrite(Laser_pin, HIGH);

    connectWifi();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    while ((timestamp = time(NULL)) < 120) {
        Serial.println("Waiting for NTP time sync: " + String(timestamp));
        delay(1000);
    }

    LINE.setToken(LINE_TOKEN);
    
    xTaskCreatePinnedToCore(PIR, "PIR", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(Tank, "Tank", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(GET_pir_command, "GET_pir_command", 10000, NULL, 1, NULL, 1);
}

void loop() {
    time(&timestamp);
}

void connectWifi() {
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
}

void Tank(void *param) {
    bool last = tank_level;
    while (1) {
        tank_level = analogRead(LDR_pin) >= LDR_threshold;
        if (last != tank_level) {
            digitalWrite(RED_pin, tank_level);
            digitalWrite(GREEN_pin, !tank_level);
            PUT_tank_level();
            if (tank_level) {
                LINE.notify("Tank Level: WARNING");
                LINE.notifyPicture("https://pbs.twimg.com/profile_images/1058485738893918208/F2B-ExKp_400x400.jpg");
            } else {
                LINE.notify("Tank Level: GOOD");
                LINE.notifyPicture("https://i.imgflip.com/1oadne.jpg");
            }
        }
        last = tank_level;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void PIR(void *param) {
    bool last = pir;
    while (1) {
        if (!PIR_on) {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        pir = digitalRead(PIR_pin);
        if (last == false && pir == true) {
            Serial.print("pet moved at time ");
            Serial.println(timestamp);
            POST_pir();
        }
        last = pir;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void PUT_tank_level() {
    const String url = baseUrl + String("update/tank_level/") + String(room_id) + String("/") + String(tank_level);
    HTTPClient http;
    http.begin(url);
    int httpCode = http.PUT("");
    if (httpCode >= 200 && httpCode <300) {
        Serial.println("PUT_tank_level OK");
    } else {
        Serial.println("PUT_tank_level ERROR");
    }
}

void POST_pir() {
    const String url = baseUrl + String("add_time");
    DynamicJsonDocument doc(2048);
    String json;
    doc["room_id"] = room_id;
    doc["count"] = count++;
    doc["timestamp"] = timestamp;
    serializeJson(doc, json);
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);
    if (httpCode >= 200 && httpCode <300) {
        Serial.println("POST_pir OK");
    } else {
        Serial.println("POST_pir ERROR");
    }
}

void GET_pir_command(void *param) {
    while (1) {
        const String url = baseUrl + String("getdata/commands/") + String(room_id);
	    String payload;
	    DynamicJsonDocument doc(2048);
	    HTTPClient http;
	    http.begin(url);
	    int httpCode = http.GET();
	    if (httpCode >= 200 && httpCode < 300) {
	    	Serial.println("GET_pir_command OK");
	    	payload = http.getString();
	    	deserializeJson(doc, payload);
	    	PIR_on = doc["PIR_on"];
	    } else {
	    	Serial.println("GET_pir_command ERROR");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
