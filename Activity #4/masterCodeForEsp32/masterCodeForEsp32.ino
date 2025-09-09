#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Replace with the MAC address of the Receiver ESP32
uint8_t receiverAddress[6] = {0x14, 0x33, 0x5C, 0x37, 0x57, 0x82};

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
    // Read angle from Serial Monitor
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      
      // Validate input (0-180)
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        SerialBT.println(input);   // Send angle to slave
        Serial.print("Sent angle: ");
        Serial.println(angle);
      } else {
        Serial.println("Invalid angle! Enter 0-180");
      }
    }
  } else {
    Serial.println("Not connected...");
    delay(2000);
  }
  
  delay(100); // Small delay to prevent flooding
}