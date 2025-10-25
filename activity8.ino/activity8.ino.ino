#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Wi-Fi credentials
const char* ssid = "Teacher's Portal";  // Replace with your Wi-Fi name
const char* password = "welcomeUBIAN";  // Replace with your Wi-Fi password

// n8n Webhook URL
const char* n8nWebhookURL = "http://172.17.0.135:5678/webhook-test/temperature-data";  // Corrected URL format

// DHT sensor setup
#define DHT_PIN 23  // GPIO pin for the DHT sensor
#define DHT_TYPE DHT11  // DHT11 sensor
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);  // Start serial communication
  dht.begin();  // Initialize the DHT sensor
  connectToWiFi();  // Connect to Wi-Fi
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToN8N();  // Read sensor data and send to n8n
  } else {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWiFi();  // Reconnect if disconnected
  }
  delay(30000);  // Wait for 30 seconds before sending data again
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendDataToN8N() {
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading from sensor!");
    return;
  }

  // Prepare JSON data
  String jsonPayload = "{\"temperature\":" + String(temperature) + ", \"humidity\":" + String(humidity) + "}";
  
  // Send data to n8n
  HTTPClient http;
  http.begin(n8nWebhookURL);  // Set the n8n webhook URL
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonPayload);
  
  if (httpCode > 0) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.println("Failed to send data.");
  }
  
  http.end();  // End the HTTP request
}
