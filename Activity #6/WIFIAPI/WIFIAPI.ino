#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4      // Data pin connected to GPIO4 (change if needed)
#define DHTTYPE DHT11 // DHT11 sensor

const char* ssid = "PLDTHOMEWIFI_Guest";
const char* password = "Ramos123456!";

const char* serverURL = "http://192.168.1.6:8000/esp32/post-data";  

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

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin(); // Initialize sensor
}

void loop() {
  sendPostRequest();
  delay(10000); 
}

void sendPostRequest() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"Temperature\":" + String(temperature, 1) +
                        ",\"Humidity\":" + String(humidity, 1) + "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
