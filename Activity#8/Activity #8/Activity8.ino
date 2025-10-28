#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ====== WiFi Credentials ======
const char* ssid = "I'm Happy";
const char* password = "Happy111!";

// ====== n8n Webhook URL (CHANGE THIS TO YOUR PRODUCTION URL) ======
const char* n8nWebhookURL = "http://192.168.1.8:5678/webhook-test/Temperature alert";

// ====== DHT Sensor Setup ======
#define DHT_PIN 5
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// ====== Device Info ======
String deviceId = "ESP32_001";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nBooting...");

  dht.begin();
  connectToWiFi();
  testN8NConnection();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    readAndSendSensorData();
  } else {
    Serial.println("⚠️ WiFi disconnected! Reconnecting...");
    connectToWiFi();
  }

  delay(30000); // 30 seconds
}

// ====== WiFi Connection ======
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
  }
}

// ====== Test Connection to n8n ======
void testN8NConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Cannot test n8n connection: WiFi not connected.");
    return;
  }

  Serial.println("🔗 Testing connection to n8n...");
  HTTPClient http;
  http.begin(n8nWebhookURL);
  http.setTimeout(5000);

  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("✅ Server responded with HTTP code: " + String(httpCode));
  } else {
    Serial.println("❌ Cannot reach n8n! Error code: " + String(httpCode));
  }
  http.end();
}

// ====== Read Sensor and Send to n8n ======
void readAndSendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Sensor read failed! Retrying next cycle...");
    return;
  }

  Serial.println("\n📊 Sensor Data:");
  Serial.println("Temperature: " + String(temperature, 1) + " °C");
  Serial.println("Humidity: " + String(humidity, 1) + " %");

  String jsonPayload = "{";
  jsonPayload += "\"deviceId\":\"" + deviceId + "\",";
  jsonPayload += "\"temperature\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humidity\":" + String(humidity, 1) + ",";
  jsonPayload += "\"timestamp\":" + String(millis());
  jsonPayload += "}";

  sendToN8N(jsonPayload);
}

// ====== Send JSON Data to n8n ======
void sendToN8N(const String& payload) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Cannot send: WiFi disconnected!");
    return;
  }

  HTTPClient http;
  http.begin(n8nWebhookURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);

  Serial.println("📤 Sending data to n8n...");
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.println("✅ Data sent successfully! HTTP Code: " + String(httpCode));
  } else {
    Serial.println("❌ Failed to send data! Error: " + String(httpCode));
  }

  http.end();
  Serial.println("====================");
}
