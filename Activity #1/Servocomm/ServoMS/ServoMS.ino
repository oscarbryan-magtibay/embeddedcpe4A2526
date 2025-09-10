#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
uint8_t receiverAddress[6] = {0x38, 0x18, 0x2B, 0x8A, 0x39, 0x7E}; 

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true);
  Serial.println("Master initialized, connecting...");

  if (SerialBT.connect(receiverAddress)) {
    Serial.println("Connected to Slave!");
  } else {
    Serial.println("Failed to connect to Slave...");
  }
}

void loop() {
  if (Serial.available()) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      SerialBT.println(angle);
      Serial.print("Sent angle: ");
      Serial.println(angle);
    } else {
      Serial.println("Enter a value between 0 and 180");
    }
  }
}
