#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "sample_SSID";
const char* password = "sample_password";  // 

String serverUrl = "_______";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // If reading fails
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Show readings
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C  | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Build JSON
  String jsonData = "{\"temperature\": " + String(temperature, 2) +
                    ", \"humidity\": " + String(humidity, 2) + "}";

  // Send POST request
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonData);

  Serial.print("POST Response Code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server Response:");
    Serial.println(response);
  } else {
    Serial.println("Error in sending POST request");
  }

  http.end();

  delay(10000); // send every 10 seconds
}
