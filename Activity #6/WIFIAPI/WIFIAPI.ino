#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4       // GPIO4
#define DHTTYPE DHT11  // DHT11 sensor

const char* ssid = "PLDTHOMEWIFI_Guest";
const char* password = "Ramos123456!";

// Your FastAPI server IP & port
const char* serverURL = "http://192.168.1.9:8000/esp32/post-data";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize DHT
  dht.begin();

  // Connect to WiFi
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
}

void loop() {
  sendPostRequest();
  delay(10000); // every 10 seconds
}

void sendPostRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Read DHT11 data
    float temperature = dht.readTemperature(); // Celsius
    float humidity = dht.readHumidity();

    // Check if readings failed
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Build JSON payload with temperature & humidity
    String jsonPayload = "{\"message\":\"Hello World!\",\"name\":\"Jay\",\"temperature\":";
    jsonPayload += temperature;
    jsonPayload += ",\"humidity\":";
    jsonPayload += humidity;
    jsonPayload += "}";

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
