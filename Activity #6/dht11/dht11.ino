#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "LOMIAN NI MOI-MOI";
const char* password = "akinkalangsherilou";

// Server details
const char* serverURL = "http://192.168.254.100:8000/esp32/post-data";

// DHT11 sensor setup
#define DHTPIN 4       // DATA pin connected to GPIO 4
#define DHTTYPE DHT11  // Sensor type

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Serial.println("✅ DHT11 sensor initialized");

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  sendSensorData();
  delay(30000); // every 30 seconds
}

void sendSensorData() {
  if (WiFi.status() == WL_CONNECTED) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Celsius

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("❌ Failed to read from DHT11 sensor");
      return;
    }

    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

    Serial.printf("Humidity: %.2f %%  Temp: %.2f °C  HeatIndex: %.2f °C\n", 
                  humidity, temperature, heatIndex);

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{";
    jsonPayload += "\"sensor_type\":\"DHT11\",";
    jsonPayload += "\"humidity\":" + String(humidity, 2) + ",";
    jsonPayload += "\"temperature\":" + String(temperature, 2) + ",";
    jsonPayload += "\"heat_index\":" + String(heatIndex, 2) + ",";
    jsonPayload += "\"unit\":\"celsius\"";
    jsonPayload += "}";

    Serial.println("📤 Sending JSON: " + jsonPayload);
    
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("✅ Server Response: " + http.getString());
    } else {
      Serial.print("❌ Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();

  } else {
    Serial.println("⚠️ WiFi not connected");
  }
}
