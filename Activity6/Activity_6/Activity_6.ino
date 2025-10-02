#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char* ssid = "PLDTHOMEFIBRfm7TH_2.4G";
const char* password = "Powppies_1921";

// FastAPI server
String serverName = "http://192.168.1.16:8000/esp32/post-data";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi!");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      delay(2000);
      return;
    }

    // Create JSON payload
    String jsonData = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}";

    // Send POST request
    HTTPClient http;
    Serial.print("Connecting to server: ");
    Serial.println(serverName);

    if (http.begin(serverName)) {
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Server Response: " + response);
      } else {
        Serial.print("POST failed, error code: ");
        Serial.println(httpResponseCode);
        Serial.println("-> Check server IP, port, or firewall");
      }

      http.end();
    } else {
      Serial.println("Unable to connect to server. Check IP and network.");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(10000); // send every 10 seconds
}
