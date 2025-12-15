#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

/* ==============================
   WiFi Credentials
   ============================== */
const char* ssid     = "Teacher's Portal";
const char* password = "welcomeUbian";

/* ==============================
   n8n Webhook
   ============================== */
const char* n8nWebhookURL =
  "http://192.168.1.21:5678/webhook/temperature-alert";

/* ==============================
   DHT Sensor Configuration
   ============================== */
#define DHT_PIN  4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

/* ==============================
   Device Info & Limits
   ============================== */
String deviceId = "ESP32_001";

int sendCount = 0;
const int maxSendCount = 1;

/* ==============================
   Setup
   ============================== */
void setup() {
  Serial.begin(115200);

  dht.begin();
  connectToWiFi();
  testN8NConnection();
}

/* ==============================
   Main Loop
   ============================== */
void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    if (sendCount < maxSendCount) {
      readAndSendSensorData();
    } else {
      Serial.println("Email limit reached. No more sends.");
    }

  } else {
    Serial.println("WiFi disconnected! Reconnecting...");
    connectToWiFi();
  }

  delay(30000);
}

/* ==============================
   WiFi Connection
   ============================== */
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
    Serial.println("\nWiFi Connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Failed!");
  }
}

/* ==============================
   Test n8n Connection
   ============================== */
void testN8NConnection() {
  Serial.println("Testing connection to n8n...");

  HTTPClient http;
  http.begin(n8nWebhookURL);
  http.setTimeout(5000);

  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.println("Server responded with: " + String(httpCode));
  } else {
    Serial.println("Cannot reach n8n! Error: " + String(httpCode));
  }

  http.end();
}

/* ==============================
   Read Sensor & Send Data
   ============================== */
void readAndSendSensorData() {
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor read failed!");
    return;
  }

  Serial.println("\nSensor Data:");
  Serial.println("Temperature: " + String(temperature) + "°C");
  Serial.println("Humidity: " + String(humidity) + "%");

  String jsonPayload = "{";
  jsonPayload += "\"deviceId\":\"" + deviceId + "\",";
  jsonPayload += "\"temperature\":" + String(temperature, 1) + ",";
  jsonPayload += "\"humidity\":" + String(humidity, 1) + ",";
  jsonPayload += "\"timestamp\":\"" + String(millis()) + "\"";
  jsonPayload += "}";

  sendToN8N(jsonPayload);
}

/* ==============================
   Send Data to n8n
   ============================== */
void sendToN8N(String payload) {
  Serial.println("Sending to n8n...");

  HTTPClient http;
  http.begin(n8nWebhookURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.println("Success! HTTP: " + String(httpCode));
    sendCount++;
    Serial.println("Sent count: " + String(sendCount));
  } else {
    Serial.println("Failed! Error: " + String(httpCode));
  }

  http.end();
  Serial.println("====================");
}
