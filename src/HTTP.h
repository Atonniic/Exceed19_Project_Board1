#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern const char *ssid;
extern const char *password;

const String baseUrl = "/";

extern int room_id;
int count = 0;
extern time_t timestamp;

extern bool tank_level;

extern bool PIR_on;

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
