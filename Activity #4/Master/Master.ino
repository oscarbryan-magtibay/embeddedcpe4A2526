#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t receiverAddress[6] = {0x00, 0x4B, 0x12, 0x9A, 0x15, 0x0A};

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true); 
  Serial.println("Master initialized, connecting...");

  if (SerialBT.connect(receiverAddress)) {
    Serial.println("Connected to Slave ESP32");
  } else {
    Serial.println("Failed to connect. Retrying...");
  }

  Serial.println("Type angle (0–180) in Serial Monitor to send.");
}

void loop() {
  if (Serial.available()) {
    String angle = Serial.readStringUntil('\n'); 
    angle.trim();

    if (SerialBT.connected()) {
      SerialBT.println(angle); 
      Serial.printf("Sent angle: %s\n", angle.c_str());
    } else {
      Serial.println("Not connected...");
    }
  }
}
