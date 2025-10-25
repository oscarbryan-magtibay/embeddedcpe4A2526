#include <WiFi.h>
#include <HTTPClient.h>

// =========================
// 🔧 CONFIGURATION
// =========================
const char* SSID = "Teacher's Portal";            // <-- Change this
const char* PASSWORD = "welcomeUBIAN";    // <-- Change this

// Replace with your FastAPI server IP and endpoint
// Example: http://192.168.1.100:8000/data
const char* RECEIVER_IP = "http://172.17.3.61:8000/data";

// =========================
// ⚙️ SETUP
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(SSID, PASSWORD);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(1000);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Failed to connect to WiFi.");
  }
}

// =========================
// 📡 FUNCTION: Send Data
// =========================
void sendPostRequest(float temperature, float distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(RECEIVER_IP);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String payload = "{\"temperature\": " + String(temperature, 2) + 
                     ", \"distance\": " + String(distance, 2) + "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.print("📨 Server Response: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    } else {
      Serial.print("❌ Error sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("⚠️ WiFi not connected");
  }
}

// =========================
// 🔁 LOOP
// =========================
void loop() {
  // Example: generate fake sensor data
  float temperature = random(20, 35);  // Replace with actual sensor reading
  float distance = random(5, 100);     // Replace with actual sensor reading

  sendPostRequest(temperature, distance);
  delay(5000); // Send data every 5 seconds
}
