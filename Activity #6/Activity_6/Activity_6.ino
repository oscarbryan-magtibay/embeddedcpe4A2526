#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 4        // GPIO pin where the DHT11 data pin is connected
#define DHTTYPE DHT11   // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Teacher's Portal";
const char* password = "welcomeUBIAN";
const char* serverURL = "http://172.17.15.12:8000/esp32/post-data";

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

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
  sendPostRequest();
  delay(10000);
}

void sendPostRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                         ",\"humidity\":" + String(humidity) + "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);

      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        const char* timestamp = doc["received"]["timestamp"];
        Serial.print("Timestamp: ");
        Serial.println(timestamp);
      }
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}