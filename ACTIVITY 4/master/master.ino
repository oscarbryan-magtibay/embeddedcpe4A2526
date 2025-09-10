#include "BluetoothSerial.h"

BluetoothSerial SerialBT;


uint8_t slaveAddress[6] = {0x80, 0xF3, 0xDA, 0x55, 0x1B, 0x7A}; 

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true); 
  Serial.println("Master ready. Connecting...");

  if (SerialBT.connect(slaveAddress)) {
    Serial.println("Connected to Slave ESP32!");
  } else {
    Serial.println("Failed to connect. Restart Master.");
  }

  Serial.println("Type an angle (0-180):");
}

void loop() {
  if (Serial.available()) {
    int angle = Serial.parseInt(); 
    if (angle >= 0 && angle <= 180) {
      SerialBT.println(angle); 
      Serial.print("Sent angle: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid angle! Please enter 0 to 180.");
    }
  }
}