#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char * SSID = "ZTE_2.4G_Medjj4";
const char * PASSWORD = "T6CEnH6m";
const char * RECEIVER_IP = "http://192.168.1.3:8000/esp32/post-data";  

#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  
  else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    sendPostRequest(temperature, humidity);
  }

  delay(10000);
}

void sendPostRequest(float temp, float hum) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(RECEIVER_IP);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"temperature\":" + String(temp, 2) + 
                         ",\"humidity\":" + String(hum, 2) + "}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);
    }
    
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  
  else {
    Serial.println("WiFi not connected");
  }
}
