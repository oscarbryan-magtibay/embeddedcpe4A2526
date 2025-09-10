//slave
#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Servo servo;
int pin = 23;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Receiver");
  servo.attach(pin);
  servo.write(90);
  Serial.println("Ready");
}

void loop() {
  if (SerialBT.available()) {
    int angle = SerialBT.parseInt();
    if (angle >= 0 && angle <= 180) {
      servo.write(angle);
      Serial.print("Got: ");
      Serial.println(angle);
    } else {
      Serial.println("Wrong angle");
    }
  }
}
