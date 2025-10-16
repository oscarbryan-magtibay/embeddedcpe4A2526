#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

const char* ssid = "hotspot";          // Your hotspot name
const char* password = "hotspotpass";  // Your hotspot password

// ✅ Change this to your computer's local IP and FastAPI  
const char* serverUrl = "http://172.20.10.2:8000/data";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.printf("Temp: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
      int httpResponseCode = http.POST(payload);

      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      http.end();
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  delay(5000); // Send every 5 seconds
}
