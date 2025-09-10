#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Servo myServo;
int servoPin = 18;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Receiver");
  myServo.attach(servoPin);
  myServo.write(90);
  Serial.println("Receiver ready, waiting for data...");
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();
    int angle = input.toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Received angle: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid data received");
    }
  }
}