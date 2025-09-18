#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t receiverAddress[6] = {0x04, 0x83, 0x08, 0x57, 0x40, 0xDA};

void setup() {
  Serial.begin(115200);
  Serial.println("Enter angle (0 - 180):");

  SerialBT.begin("ESP32_ServoSender", true);
  if (SerialBT.connect(receiverAddress)) {
    Serial.println("Connected to Receiver ESP32!");
  } else {
    Serial.println("Failed to connect. Retrying...");
  }
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        SerialBT.println(angle);
        Serial.print("Sent angle: ");
        Serial.println(angle);
      } else {
        Serial.println("Invalid input! Enter value 0 - 180.");
      }
    }
  }
}