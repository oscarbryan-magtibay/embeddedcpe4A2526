#HTML
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Weather Dashboard</title>
<style>
  body {
    margin: 0;
    font-family: Arial, sans-serif;
    background: linear-gradient(to bottom, #87CEEB, #f0f8ff);
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    color: #333;
  }
  .weather-card {
    background: rgba(255, 255, 255, 0.9);
    padding: 30px 50px;
    border-radius: 20px;
    box-shadow: 0 8px 20px rgba(0,0,0,0.2);
    text-align: center;
  }
  .weather-card h1 {
    font-size: 48px;
    margin: 0 0 20px 0;
  }
  .weather-card p {
    font-size: 24px;
    margin: 10px 0;
  }
  .temp {
    color: #FF4500;
    font-weight: bold;
  }
  .humidity {
    color: #1E90FF;
    font-weight: bold;
  }
</style>
</head>
<body>
  <div class="weather-card">
    <h1>Current Weather</h1>
    <p>Temperature: <span class="temp">{{ $json.body.temperature }}°C</span></p>
    <p>Humidity: <span class="humidity">{{ $json.body.humidity }}%</span></p>
  </div>
</body>
</html>

#ARDUINO
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include "Adafruit_Sensor.h"

// --- START: USER CONFIGURATION ---

// WiFi Credentials
const char* ssid = "GlobeAtHome_fbbd0_2.4";
const char* password = "D2kDeDWc";

// n8n Webhook URL (Updated with your IP and the path "esp32-temp")
const char* n8n_webhook_url = "http://192.168.254.119:5678/webhook-test/esp32-temp";

// Sensor Configuration
#define DHTPIN 4       // Pin connected to the DHT sensor's Data pin (GPIO 4)
#define DHTTYPE DHT11    // !! PAKI-CHECK: Palitan mo ng DHT22 kung 'yun ang gamit mo !!

// --- END: USER CONFIGURATION ---

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("DHT11 Temperature & Humidity Monitor");
  dht.begin();

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
    if (wifi_retries > 30) {
      Serial.println("\nFailed to connect to WiFi. Restarting...");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Wait 30 seconds between readings
  Serial.println("\nWaiting 30 seconds for next reading...");
  delay(30000); 

  // Read temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Read in Celsius

  // Check if readings are valid
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return; // Exit loop and try again
  }

  Serial.print("Sensor Reading: ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  // Create JSON payload
  // Using sprintf for efficient String formatting
  char jsonPayload[100];
  sprintf(jsonPayload, "{\"temperature\":%.1f, \"humidity\":%.1f}", t, h);
  
  // Send data to n8n Webhook
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(n8n_webhook_url);
    http.addHeader("Content-Type", "application/json");

    Serial.println("Sending data to n8n...");
    int httpResponseCode = http.POST(String(jsonPayload));

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      
      // Get the response (which will be our HTML)
      String response = http.getString();
      Serial.println("--- n8n HTML Response ---");
      Serial.println(response);
      Serial.println("-------------------------");

    } else {
      Serial.print("Error sending POST request: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Free resources
  } else {
    Serial.println("WiFi is disconnected.");
  }
}