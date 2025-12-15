#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "TPz";
const char* password = "CONFIRM32138216";

// Backend server URL
const char* serverUrl = "http://192.168.254.110:5000/api/sensor-data";

// Sensor pins
#define SOIL_SENSOR_PIN 34
#define DHT_PIN 22
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// Timing
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds

// Calibration values for soil sensor
const int DRY_VALUE = 3500;   // Adjust based on your sensor
const int WET_VALUE = 1500;   // Adjust based on your sensor

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Test soil sensor calibration
  Serial.println("Calibrating soil sensor...");
  Serial.print("Initial reading: ");
  Serial.println(analogRead(SOIL_SENSOR_PIN));
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Test server connection
  testServerConnection();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read sensors
    int soilMoisture = analogRead(SOIL_SENSOR_PIN);
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Convert soil moisture to percentage WITH CALIBRATION
    int soilMoisturePercent = map(soilMoisture, DRY_VALUE, WET_VALUE, 0, 100);
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
    
    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      humidity = 0;
      temperature = 0;
    }
    
    // Debug print
    Serial.print("Soil Raw: ");
    Serial.print(soilMoisture);
    Serial.print(" -> ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");
    
    // Prepare JSON data
    String jsonData = "{";
    jsonData += "\"soil_moisture\":" + String(soilMoisture) + ",";
    jsonData += "\"soil_moisture_percent\":" + String(soilMoisturePercent) + ",";
    jsonData += "\"temperature\":" + String(temperature) + ",";
    jsonData += "\"humidity\":" + String(humidity);
    jsonData += "}";
    
    // Send data to server
    sendDataToServer(jsonData);
    
    // Print to serial for debugging
    Serial.println("Data sent:");
    Serial.println(jsonData);
  }
}

void sendDataToServer(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    Serial.print("Connecting to: ");
    Serial.println(serverUrl);
    
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Set timeout (important!)
    http.setTimeout(5000);
    
    int httpResponseCode = http.POST(data);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error in HTTP request: " + String(httpResponseCode));
      Serial.println("Error details: " + http.errorToString(httpResponseCode));
      
      // Print WiFi status
      Serial.print("WiFi Status: ");
      Serial.println(WiFi.status());
      
      // Print RSSI (signal strength)
      Serial.print("Signal Strength (RSSI): ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
    // Try to reconnect
    WiFi.begin(ssid, password);
  }
}

void testServerConnection() {
  Serial.println("Testing server connection...");
  
  HTTPClient http;
  String testUrl = "http://192.168.254.110:5000";
  http.begin(testUrl);
  http.setTimeout(3000);
  
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.printf("✓ Server responded with code: %d\n", httpCode);
    String payload = http.getString();
    Serial.println("Response: " + payload);
  } else {
    Serial.printf("✗ Connection failed, error: %s\n", http.errorToString(httpCode).c_str());
    
    // Try alternative - maybe server is on different port?
    Serial.println("Trying common ports...");
    int ports[] = {5000, 8000, 8080, 80};
    for (int port : ports) {
      String altUrl = "http://192.168.254.110:" + String(port);
      http.begin(altUrl);
      http.setTimeout(1000);
      if (http.GET() > 0) {
        Serial.println("Found server at: " + altUrl);
        break;
      }
    }
  }
  http.end();
}