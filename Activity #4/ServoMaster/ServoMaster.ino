#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t slaveAddress[6] = {0xEC, 0xE3, 0x34, 0x1B, 0x5E, 0xB6};

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true);
  Serial.println("Master is ready. Connecting to Slave...");

  while (!SerialBT.connect(slaveAddress)) {
    Serial.println("Failed to connect to Slave. Retrying...");
    delay(2000);
  }

  Serial.println("Connected to Slave!");
}

void loop() {
  if (SerialBT.connected()) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();

      int angle = input.toInt();

      if (angle >= 0 && angle <= 180) {
        SerialBT.println(angle);
        Serial.printf("Sent angle: %d\n", angle);
      }
      
      else {
        Serial.println("Invalid input! Enter a number between 0–180.");
      }
    }
  }
  
  else {
    Serial.println("Disconnected. Reconnecting to Slave...");
    while (!SerialBT.connect(slaveAddress)) {
      Serial.println("Retrying...");
      delay(2000);
    }

    Serial.println("Reconnected!");
  }
}
