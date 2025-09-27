// DHT11_to_FastAPI.ino
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11

const char* ssid = "SKYW_6156_2G";
const char* password = "Nndp5uHb";
const char* serverURL = "http://192.168.1.6:8000/esp32/post-data";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  dht.begin();

  Serial.print("Connecting to WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected:");
  Serial.println(WiFi.localIP());
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.printf("T: %.2f °C, H: %.2f %%\n", t, h);
    sendPostRequest(t, h);
  }

  delay(10000); // send every 10s
}

void sendPostRequest(float temperature, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{";
  payload += "\"temperature\": " + String(temperature, 2) + ",";
  payload += "\"humidity\": " + String(humidity, 2) + ",";
  payload += "\"device\": \"esp32_dht11\"";
  payload += "}";

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.printf("POST %d\n%s\n", httpCode, response.c_str());
  } else {
    Serial.printf("Error on sending POST: %d\n", httpCode);
  }

  http.end();
}
