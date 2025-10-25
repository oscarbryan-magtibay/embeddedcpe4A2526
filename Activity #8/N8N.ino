#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"


#define DHTPIN 4        
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


const char* ssid = "AIR FURS ONE1";
const char* password = "Skipmakliw041*";


const char* webhook_url = "http://192.168.1.10:5678/webhook-test/esp32-temp";


unsigned long lastSendTime = 0;
const unsigned long sendInterval = 30000; 

void connectToWiFi() {
  Serial.print("📶 Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retry++;
    if (retry > 40) { 
      Serial.println("\n⚠️ WiFi connection failed, retrying...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      retry = 0;
    }
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("🌐 IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi lost. Reconnecting...");
    connectToWiFi();
  }


  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= sendInterval) {
    lastSendTime = currentTime;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("⚠️ Failed to read from DHT sensor!");
      return;
    }

    Serial.print("🌡 Temp: ");
    Serial.print(temperature);
    Serial.print("°C  💧 Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(webhook_url);
      http.addHeader("Content-Type", "application/json");

      
      String payload = "{\"temperature\": " + String(temperature, 1) +
                       ", \"humidity\": " + String(humidity, 1) + "}";

      int httpResponseCode = http.POST(payload);

      Serial.print("📡 POST -> ");
      Serial.println(payload);
      Serial.print("📬 Response code: ");
      Serial.println(httpResponseCode);

      http.end();
    } else {
      Serial.println("❌ Skipped POST: WiFi not connected");
    }
  }
}
