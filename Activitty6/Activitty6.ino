#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// --- DHT Sensor Setup ---
#define DHTPIN 23          // Pin connected to DHT sensor
#define DHTTYPE DHT11      // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// --- WiFi Credentials ---
const char* ssid = "Lab2";
const char* password = "Lab2@2023";

// --- FastAPI Server URL ---
const char* serverURL = "http://192.168.30.137:8000/esp32/post-data";

void setup() {
  Serial.begin(115200);
  dht.begin();

  // --- Connect to WiFi ---
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // --- Read DHT Sensor ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // --- Display Readings ---
  Serial.println("📤 Sending data to FastAPI server...");
  Serial.print("🌡 Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | 💧 Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // --- Send Data to FastAPI ---
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String json = "{\"temperature\": " + String(temperature, 2) +
                  ", \"humidity\": " + String(humidity, 2) + "}";

    int httpResponseCode = http.POST(json);

    // --- Server Response ---
    if (httpResponseCode > 0) {
      Serial.print("✅ Server Response (");
      Serial.print(httpResponseCode);
      Serial.println("):");
      Serial.println(http.getString());
    } else {
      Serial.print("❌ Error Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("⚠️ WiFi disconnected!");
  }

  delay(10000);  // Send data every 10 seconds
}
