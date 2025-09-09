#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("MAU_ESP32_LED_Receiver"); 
  Serial.println("Receiver ready. Waiting for Master...");
  Serial.println("Receiver Bluetooth MAC address:");
  Serial.println(SerialBT.getBtAddressString().c_str());
}
void loop(){ 
  delay(1000); 
}

