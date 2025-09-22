#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4        
#define DHTTYPE DHT11   

const char* ssid = "WIFI WII";
const char* password = "noodlldes";

const char* serverURL = "http://10.63.40.21:8080/esp32/post-data";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(10000);
    return;
  }

  bool posted = false;
  int attempt = 1;

  while (!posted) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverURL);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = "{\"temperature\":" + String(temperature, 2) +
                           ",\"humidity\":" + String(humidity, 2) + "}";

      int httpResponseCode = http.POST(jsonPayload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Server Response:");
        Serial.println(response);
        posted = true;
      } else {
        Serial.print("POST failed, attempt ");
        Serial.println(attempt);
        attempt++;
        delay(3000); 
      }

      http.end();
    } else {
      Serial.println("WiFi not connected, retrying...");
      delay(3000);
    }
  }

  delay(10000); 
}
