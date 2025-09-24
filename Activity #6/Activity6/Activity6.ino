#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 23       
#define DHTTYPE DHT11  

const char* ssid = "Coolit2.4";
const char* password = "Macoolit20";

const char* serverName = "http://192.168.1.6:8000/esp32/post-data";  

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Sending -> Temp: ");
    Serial.print(t);
    Serial.print(" Humidity: ");
    Serial.println(h);

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"Temperature\": " + String(t) + ", \"Humidity\": " + String(h) + "}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(15000);  
}
