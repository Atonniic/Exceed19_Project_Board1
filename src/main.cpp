#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

#define RED_pin 25
#define GREEN_pin 26
#define PIR_pin 27
#define Laser_pin 33
#define LDR_pin 34

const int room_id = 0;

const char *ssid = "Pun-iPhone";
const char *password = "spiderman";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

const String baseUrl = "/";

time_t timestamp;
int count = 0;
bool pir = false; // true -> pet moved, false -> pet not moved
bool tank_level = false; // true -> refill needed, false -> refill not needed
bool PIR_on = true;

void Tank(void *param);
void PIR(void *param);
void connectWifi();
void POST_pir();
void PUT_tank_level();
void GET_pir_command(void *param);

void setup() {
    Serial.begin(115200);

    pinMode(RED_pin, OUTPUT);
    pinMode(GREEN_pin, OUTPUT);
    pinMode(PIR_pin, INPUT);
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
    xTaskCreatePinnedToCore(GET_pir_command, "GET_pir_command", 10000, NULL, 1, NULL, 1);
}

void loop() {
    time(&timestamp);
}

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

void connectWifi() {
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
}

void POST_pir() {
    const String url = baseUrl + String("timestamp");
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
    if (httpCode > 200 && httpCode <300) {
        Serial.println("POST_pir OK");
    } else {
        Serial.println("POST_pir ERROR");
    }
}

void PUT_tank_level() {
    const String url = baseUrl + String("tank_level") + String(room_id) + String("/") + String(tank_level);
    HTTPClient http;
    http.begin(url);
    int httpCode = http.PUT("");
    if (httpCode > 200 && httpCode <300) {
        Serial.println("PUT_tank_level OK");
    } else {
        Serial.println("PUT_tank_level ERROR");
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
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
