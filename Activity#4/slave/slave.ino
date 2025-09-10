#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Servo myServo;

const int servoPin = 23;
int currentAngle = 90;
void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Receiver");
  myServo.attach(servoPin);
  myServo.write(currentAngle);
  Serial.println("Receiver ready. Waiting for angles...");
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n'); 
    input.trim();
    if (input.length() > 0) {
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        currentAngle = angle;
        myServo.write(currentAngle);
        Serial.printf("Servo moved to %d degrees\n", currentAngle);
      } else {
        Serial.printf("Invalid value received: %s\n", input.c_str());
      }
    }
  }
}
