#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
uint8_t receiverAddress[6] = {0x80, 0xF3, 0xDA, 0x55, 0x7D, 0xA6};

void setup() {
  // Change pins for Serial2
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  
  SerialBT.begin("ESP32_Master", true);
  Serial2.println("Master initialized, connecting...");
  if (SerialBT.connect(receiverAddress)) {
    Serial2.println("Connected to Receiver ESP32!");
  } else {
    Serial2.println("Failed to connect. Retrying...");
  }
}

void loop() {
  if (SerialBT.connected()) {
    if (Serial2.available()) {
      String input = Serial2.readStringUntil('\n');
      input.trim();
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        SerialBT.println(input);
        Serial2.print("Sent angle: ");
        Serial2.println(angle);
      } else {
        Serial2.println("Invalid angle! Enter 0–180");
      }
    }
  } else {
    Serial2.println("Not connected...");
    delay(2000);
  }
  delay(100);
}

