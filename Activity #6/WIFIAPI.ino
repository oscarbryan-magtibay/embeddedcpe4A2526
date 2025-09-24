#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char* ssid = "AIR FURS ONE1";
const char* password = "Skipmakliw041*";

// Replace with your PC IP (found with ipconfig)
const char* serverURL = "http://192.168.1.3:8000/esp32/post-data";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"Temperature\":" + String(temp) + ",\"Humidity\":" + String(hum) + "}";
    int code = http.POST(payload);

    if (code > 0) {
      String resp = http.getString();
      Serial.print("HTTP ");
      Serial.print(code);
      Serial.print(" - ");
      Serial.println(resp);
    } else {
      Serial.print("POST error: ");
      Serial.println(code);
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }
  delay(10000); // 10s between readings
}
