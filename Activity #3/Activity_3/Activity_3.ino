#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 2;
const float SERVO_SPEED = 360.0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo_Control");
  Serial.println("Bluetooth Device is Ready to Pair");

  myServo.attach(SERVO_PIN, 1000, 2000);
  myServo.write(90);
}

void rotateDegrees(int angle) {
  int direction = (angle >= 0) ? 180 : 0;
  int degrees = abs(angle);

  int duration = (degrees / SERVO_SPEED) * 1000;

  myServo.write(direction);
  delay(duration);
  myServo.write(90);
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    int angle = input.toInt();
    if (angle >= -360 && angle <= 360) {
      rotateDegrees(angle);
      Serial.printf("Rotated %d degrees\n", angle);
      SerialBT.printf("Rotated %d degrees\n", angle);
    } else {
      Serial.println("Invalid input! Enter 0 to 180.");
      SerialBT.println("Invalid input! Enter 0 to 180.");
    }
  }
}
