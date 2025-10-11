#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "PLDTHOMEFIBRfm7TH_2.4G";    
const char* password = "Powppies_1921";     

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://192.168.1.16:8000/esp32/post-data");
    http.addHeader("Content-Type", "application/json");


    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      Serial.print("Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }

  delay(10000); 
}
