/* Master_ESPNow_Sender.ino
   Sends a single-byte angle (0..180) to Slave via ESP-NOW.
   Supports Serial input (type 0-180 + Enter) and optional potentiometer on GPIO36.
*/
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const bool USE_POT = false;   // set true if you wired a pot to potPin
const int potPin = 36;        // ADC pin for pot (GPIO36)
const int sendIntervalMs = 100; // pot send interval

// <-- Replace with your slave's MAC printed from its Serial Monitor:
uint8_t slaveMac[6] = {0xFC, 0xE8, 0xC0, 0xE0, 0xB3, 0x74}; // <--- CHANGE this

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while(1) delay(1000);
  }
  esp_now_register_send_cb(onSent);

  // Add peer (the slave)
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, slaveMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
  } else {
    Serial.print("Peer added: ");
    for (int i=0;i<6;i++){ Serial.printf("%02X", slaveMac[i]); if(i<5) Serial.print(":"); }
    Serial.println();
  }

  Serial.println("Master ready. Type angle 0-180 or enable pot.");
  if (USE_POT) {
    Serial.println("Pot mode enabled.");
  } else {
    Serial.println("Serial mode enabled.");
  }
}

unsigned long lastSend = 0;

void loop() {
  // SERIAL INPUT MODE
  if (!USE_POT) {
    if (Serial.available()) {
      String s = Serial.readStringUntil('\n');
      s.trim();
      if (s.length()) {
        int angle = s.toInt();
        if (angle >= 0 && angle <= 180) {
          uint8_t payload[1] = { (uint8_t)angle };
          esp_err_t res = esp_now_send(slaveMac, payload, sizeof(payload));
          Serial.print("Sent angle: ");
          Serial.println(angle);
        } else {
          Serial.println("Enter 0-180");
        }
      }
    }
  } 
  // POTENTIOMETER MODE
  else {
    if (millis() - lastSend >= sendIntervalMs) {
      lastSend = millis();
      int raw = analogRead(potPin); // 0-4095
      int angle = map(raw, 0, 4095, 0, 180);
      uint8_t payload[1] = { (uint8_t)angle };
      esp_now_send(slaveMac, payload, sizeof(payload));
      Serial.print("Pot -> angle ");
      Serial.println(angle);
    }
  }
}
