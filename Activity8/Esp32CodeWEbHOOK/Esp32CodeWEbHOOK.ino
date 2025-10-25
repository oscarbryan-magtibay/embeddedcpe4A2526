#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "TPz";
const char* password = "CONFIRM32138216";
const char* webhookUrl = "https://paulopaulo.app.n8n.cloud/webhook-test/1631a44d-f708-4561-956b-d739481dc286";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhookUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
    int httpResponseCode = http.POST(payload);

    Serial.println("Response code: " + String(httpResponseCode));
    http.end();
  }
  delay(10000); // every 10 seconds
}
