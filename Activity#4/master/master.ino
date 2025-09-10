#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t receiverAddress[6] = {0x80, 0xF3, 0xDA, 0x55, 0x86, 0x72};

void connectToReceiver() {
  Serial.print("Connecting to receiver...");
  if (SerialBT.connect(receiverAddress)) {
    Serial.println(" connected!");
  } else {
    Serial.println(" failed. Retrying...");
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true);
  connectToReceiver();
}

void loop() {
  if (!SerialBT.connected()) {
    Serial.println("Receiver not connected. Retrying...");
    connectToReceiver();
    delay(2000);
    return;
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int angle = input.toInt();

    if (angle >= 0 && angle <= 180) {
      SerialBT.println(angle); 
      Serial.printf("Sent angle: %d\n", angle);
    } else {
      Serial.println("Invalid angle! Enter 0–180.");
    }
  }
}
