#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Receiver MAC Address: 80:F3:DA:54:DC:5E
const uint8_t receiverAddress[6] = {0x80, 0xF3, 0xDA, 0x54, 0xDC, 0x5E};

void connectToReceiver() {
  Serial.print("Connecting to receiver...");
  if (SerialBT.connect(receiverAddress)) {
    Serial.println(" success!");
  } else {
    Serial.println(" failed. Retrying not implemented.");
  }
}

void sendAngle(int angle) {
  SerialBT.println(angle);
  Serial.printf("Sent angle: %d\n", angle);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Enter angle (0 - 180):");

  SerialBT.begin("ESP32_ServoSender", true);  // Master mode
  connectToReceiver();
}

void loop() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.isEmpty()) return;

  int angle = input.toInt();
  if (angle >= 0 && angle <= 180) {
    sendAngle(angle);
  } else {
    Serial.println("Invalid input! Enter value 0 - 180.");
  }
}
