#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Teacher's Portal";
const char* password = "welcomeUBIAN";

// n8n webhook URL - UPDATE 192.168.1.100 WITH YOUR ACTUAL IP!
const char* n8nWebhookURL = "http://192.168.100.108:5678/webhook-test/temperature-data";

// DHT sensor
#define DHT_PIN 23
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

String deviceId = "ESP32_001";

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
  
  // Test n8n connection
  testN8NConnection();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    readAndSendSensorData();
  } else {
    Serial.println("WiFi disconnected! Reconnecting...");
    connectToWiFi();
  }
  delay(30000); // 30 seconds
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("📡 ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Failed!");
  }
}

void testN8NConnection() {
  Serial.println("🔍 Testing connection to n8n...");
  Serial.println("URL: " + String(n8nWebhookURL));
  
  HTTPClient http;
  http.begin(n8nWebhookURL);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode == 405) {
    Serial.println("✅ n8n server is reachable!");
  } else if (httpCode > 0) {
    Serial.println("✅ Server responded with: " + String(httpCode));
  } else {
    Serial.println("❌ Cannot reach n8n! Error: " + String(httpCode));
    Serial.println("🔧 Please check:");
    Serial.println("1. Computer IP address in webhook URL");
    Serial.println("2. n8n is running on port 5678");
    Serial.println("3. Windows Firewall allows port 5678");
  }
  http.end();
}

void readAndSendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Sensor read failed!");
    return;
  }

  Serial.println("\n=== 📊 Sensor Data ===");
  Serial.println("🌡️  Temperature: " + String(temperature) + "°C");
  Serial.println("💧 Humidity: " + String(humidity) + "%");

  String jsonPayload = "{";
  jsonPayload += "\"deviceId\":\"" + deviceId + "\",";
  jsonPayload += "\"temperature\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humidity\":" + String(humidity, 1) + ",";
  jsonPayload += "\"timestamp\":\"" + String(millis()) + "\"";
  jsonPayload += "}";

  sendToN8N(jsonPayload);
}

void sendToN8N(String payload) {
  HTTPClient http;
  
  Serial.println("🔄 Sending to n8n...");
  
  http.begin(n8nWebhookURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);
  
  int httpCode = http.POST(payload);
  
  if (httpCode > 0) {
    Serial.println("✅ Success! HTTP: " + String(httpCode));
  } else {
    Serial.println("❌ Failed! Error: " + String(httpCode));
    Serial.println("Error: " + http.errorToString(httpCode));
  }
  
  http.end();
  Serial.println("====================");
}