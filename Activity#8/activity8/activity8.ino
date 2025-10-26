#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// ==== CONFIGURATION ==== //
#define DHTPIN 15          // DHT11 data pin
#define DHTTYPE DHT11

const char* ssid = "SKYW_6156_2G";
const char* password = "Nndp5uHb";
const char* webhookURL = "http://192.168.1.51:5678/webhook-test/temperature-webhook"; // <-- ilagay dito ang n8n URL

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhookURL);
    http.addHeader("Content-Type", "application/json");

    float temperature = dht.readTemperature(); // Celsius
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      delay(5000);
      return;
    }

    // Create JSON payload
    String payload = "{";
    payload += "\"device\":\"esp32-1\",";
    payload += "\"temp_c\":" + String(temperature, 1) + ",";
    payload += "\"humidity\":" + String(humidity, 1) + ",";
    payload += "\"timestamp\":\"" + String(millis()) + "\"";
    payload += "}";

    int httpResponseCode = http.POST(payload);
    Serial.print("POST Response: ");
    Serial.println(httpResponseCode);
    Serial.println(payload);

    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }

  delay(10000); // send every 10 seconds
}
