#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  // Use Serial2 instead of default Serial (pins can be changed)
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  

  if (!SerialBT.begin("ESP32_BT_Node")) {   // changed name
    Serial2.println("Bluetooth init failed");
  } else {
    Serial2.println("Receiver ready");
    Serial2.println(SerialBT.getBtAddressString());
  }
}

void loop() {
  delay(5000);
}