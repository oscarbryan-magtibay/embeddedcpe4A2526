#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Wi-Fi credentials
const char* ssid = "I'm Happy";
const char* password = "Happy111!";

// FastAPI server URL
const char* serverURL = "http://192.168.1.15:8000/esp32/post-data";

// DHT11 settings
#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nConnecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Failed to connect to Wi-Fi.");
  }

  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temp = dht.readTemperature();  // Celsius
    float hum = dht.readHumidity();      // Percentage

    if (isnan(temp) || isnan(hum)) {
      Serial.println("⚠️ Failed to read from DHT sensor.");
      return;
    }

    Serial.print("🌡 Temperature: ");
    Serial.print(temp);
    Serial.print(" °C | 💧 Humidity: ");
    Serial.print(hum);
    Serial.println(" %");

    sendPostRequest(temp, hum);
  } else {
    Serial.println("🔄 Wi-Fi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }

  delay(10000); // Send data every 10 seconds
}

void sendPostRequest(float temperature, float humidity) {
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  // MATCH your FastAPI model key names exactly
  String jsonPayload = "{\"Temperature\":" + String(temperature, 2) +
                       ",\"Humidity\":" + String(humidity, 2) + "}";

  Serial.print("📤 Sending JSON: ");
  Serial.println(jsonPayload);

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("✅ Server Response:");
    Serial.println(response);
  } else {
    Serial.print("❌ POST failed. Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
