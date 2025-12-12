#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4        
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "WIFI WII";
const char* password = "noodlldes";

const char* n8n_webhook = "http://10.224.240.21:5678/webhook-test/esp32-weather";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.printf(" Temp: %.2f °C |  Humidity: %.2f %%\n", temperature, humidity);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(n8n_webhook);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\": " + String(temperature) +
                     ", \"humidity\": " + String(humidity) + "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println(" Data sent successfully: " + String(httpResponseCode));
    } else {
      Serial.println(" Error sending data: " + String(httpResponseCode));
    }

    http.end();
  }

  delay(10000); 
}
