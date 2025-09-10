#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  if (!SerialBT.begin("ESP32_Receiver")) {
    Serial.println("Bluetooth init failed");
  } else {
    Serial.println("Receiver ready");
    Serial.println(SerialBT.getBtAddressString());
  }
}

void loop() {
  delay(5000);
}