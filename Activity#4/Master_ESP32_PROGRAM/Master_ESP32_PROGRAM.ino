#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
uint8_t receiverAddress[6] = {0x80, 0xF3, 0xDA, 0x55, 0x7D, 0xA6};

void setup() {
  Serial.begin(115200);
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
      String input = Serial.readStringUntil('\n');
      input.trim();
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        SerialBT.println(input);
        Serial.print("Sent angle: ");
        Serial.println(angle);
      } else {
        Serial.println("Invalid angle! Enter 0–180");
      }
    }
  } else {
    Serial.println("Not connected...");
    delay(2000);
  }
  delay(100);
}
