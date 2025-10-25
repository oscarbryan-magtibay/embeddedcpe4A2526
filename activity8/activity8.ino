#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "Alas kulit";
const char* password = "alasthenika2022";

// n8n Webhook URL
const char* n8n_webhook_url = "https://tels.app.n8n.cloud/webhook/esp32-temp";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Fake ESP32 Weather Data Sender");

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
  delay(30000);  // Send every 30 seconds

  // Fake random data
  float t = random(250, 320) / 10.0;  // 25.0–32.0 °C
  float h = random(500, 800) / 10.0;  // 50.0–80.0 %

  Serial.print("Fake Sensor Reading: ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  char jsonPayload[100];
  sprintf(jsonPayload, "{\"temperature\":%.1f, \"humidity\":%.1f}", t, h);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(n8n_webhook_url);
    http.addHeader("Content-Type", "application/json");

    Serial.println("Sending data to n8n...");
    int httpResponseCode = http.POST(String(jsonPayload));

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending POST request: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
