#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t receiverAddress[6] = {0x28, 0x56, 0x2F, 0x49, 0xFE, 0xB2};

void setup() {
  Serial.begin(115200);

  // Initialize Master mode
  SerialBT.begin("ESP32_Master", true);
  Serial.println("Master initialized, connecting...");

  if (SerialBT.connect(receiverAddress)) {
    Serial.println("Connected to Receiver ESP32!");
  } else {
    Serial.println("Failed to connect. Retrying...");
  }
}

void loop() {
  if (SerialBT.connected()) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n'); // Read user input
      input.trim();
      int angle = input.toInt();

      if (angle >= 0 && angle <= 180) {
        SerialBT.println(angle); // Send angle to slave
        Serial.printf("Sent angle: %d\n", angle);
      } else {
        Serial.println("Invalid angle! Enter 0–180.");
      }
    }
  } else {
    Serial.println("Not connected...");
    delay(2000);
  }
}
