/* Slave_ESPNow_Servo.ino
   Receives a single-byte angle (0..180) via ESP-NOW and writes it to a servo.
*/
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

Servo myServo;
const int SERVO_PIN = 18; // servo signal pin on slave

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len >= 1) {
    uint8_t angle = incomingData[0];
    if (angle <= 180) {
      myServo.write(angle);
      Serial.print("Received angle: ");
      Serial.println(angle);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Servo
  myServo.attach(SERVO_PIN);
  myServo.write(0); // initial position

  // WiFi + ESP-NOW init
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // recommended
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while(1) delay(1000);
  }

  // Register receive callback
  esp_now_register_recv_cb([](const uint8_t * mac, const uint8_t *incomingData, int len){
    onDataRecv(mac,incomingData,len);
  });

  // Print this device MAC for pairing (Master needs this MAC)
  Serial.print("Slave MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // nothing else needed; all work done in callback
  delay(10);
}
