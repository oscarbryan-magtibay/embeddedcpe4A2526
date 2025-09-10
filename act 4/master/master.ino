//master
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
uint8_t addr[] = {0x80, 0xF3, 0xDA, 0x55, 0x10, 0x0A};

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Master", true);
  Serial.println("Starting...");
  if (SerialBT.connect(addr)) {
    Serial.println("Connected!");
  } else {
    Serial.println("Connection failed!");
  }
}

void loop() {
  if (SerialBT.connected() && Serial.available()) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      SerialBT.println(angle);
      Serial.print("Sent: ");
      Serial.println(angle);
    } else {
      Serial.println("Angle 0-180 only");
    }
  } else if (!SerialBT.connected()) {
    Serial.println("Not connected");
    delay(2000);
  }
  delay(100);
}
