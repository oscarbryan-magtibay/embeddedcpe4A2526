#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo_Slave");  
  Serial.println("Slave ready. Waiting for Master...");
  Serial.print("Receiver Bluetooth MAC address: ");
  Serial.println(SerialBT.getBtAddressString()); 
}

void loop() {
  delay(1000);  
}
