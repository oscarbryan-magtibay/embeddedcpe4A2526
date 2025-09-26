#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "TPz";
const char* password = "Confirm32138216";

// Server details
const char* serverURL = "http://192.168.254.121:8000/esp32/post-data";

// DHT11 sensor setup
#define DHTPIN 4          // Digital pin connected to DHT11
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize DHT sensor
  dht.begin();
  
  Serial.println("DHT11 sensor initialized");

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  sendSensorData();
  delay(30000); // Wait 30 seconds between readings
}

void sendSensorData() {
  if (WiFi.status() == WL_CONNECTED) {
    // Read temperature and humidity from DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Celsius

    // Check if any reads failed
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Calculate heat index (feels like temperature)
    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\t");
    Serial.print("Heat index: ");
    Serial.print(heatIndex);
    Serial.println(" °C");

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload with sensor data
    String jsonPayload = "{";
    jsonPayload += "\"sensor_type\":\"DHT11\",";
    jsonPayload += "\"humidity\":" + String(humidity, 2) + ",";
    jsonPayload += "\"temperature\":" + String(temperature, 2) + ",";
    jsonPayload += "\"heat_index\":" + String(heatIndex, 2) + ",";
    jsonPayload += "\"unit\":\"celsius\"";
    jsonPayload += "}";

    Serial.println("Sending JSON: " + jsonPayload);
    
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}