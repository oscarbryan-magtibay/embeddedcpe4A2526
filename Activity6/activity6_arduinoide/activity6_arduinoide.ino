#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 23  
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Jhon Paul's Wi-Fi Network";
const char* password = "Ziki2412";

const char* serverURL = "http://192.168.100.108:8000/esp32/post-data";

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
  Serial.println("Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.println("Sending data to FastAPI server...");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

   
    String json = "{\"temperature\": " + String(temperature, 2) + 
                  ", \"humidity\": " + String(humidity, 2) + "}";


    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      Serial.print("Server Response (");
      Serial.print(httpResponseCode);
      Serial.println("):");
      Serial.println(http.getString());
    } else {
      Serial.print("Error Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected!");
  }

  delay(10000);
}
