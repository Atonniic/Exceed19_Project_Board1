/*
    All
*/
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid;
const char *password;

const char *baseUrl = "/";
int room_id = 0;
int count = 0;
extern time_t timestamp;

void connectWifi() {
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

void POST_pir() {
    const String url = baseUrl + "timestamp";
    DynamicJsonDocument doc(2048);
    String json;
    doc["room_id"] = room_id;
    doc["count"] = count++;
    doc["timestamp"] = timestamp;
    serializeJson(doc, json);
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    httpCode = http.POST(json);
    if (httpCode > 200 && httpCode <300) {
        Serial.println("POST OK");
    } else {
        Serial.println("POST ERROR");
    }
}

void GET_commands(void *param) {
    while (1) {
        const String url = baseUrl + "getdata/commands/" + String(room_id);
	    String payload;
	    DynamicJsonDocument doc(2048);
	    HTTPClient http;
	    http.begin(url);
	    int httpCode = http.GET();
	    if (httpCode >= 200 && httpCode < 300) {
	    	Serial.println("GET OK");
	    	payload = http.getString();
	    	deserializeJson(doc, payload);
	    	PIR_on = doc["PIR_on"];
	    } else {
	    	Serial.println("GET ERROR");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
} 

void POST_tank_level() {
    const String url = baseUrl + "tank_level" + String(room_id) + "/" + String(tank_level);
    HTTPClient http;
    http.begin(url);
    httpCode = http.POST();
    if (httpCode > 200 && httpCode <300) {
        Serial.println("POST OK");
    } else {
        Serial.println("POST ERROR");
    }
}