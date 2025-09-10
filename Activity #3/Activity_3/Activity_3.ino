#include <ESP32Servo.h>

#define SERVO_PIN 23
Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
}

void loop() {
  if (Serial.available() > 0) {
    int angle = Serial.readStringUntil('\n').toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.printf("Servo moved to %d degrees.\n", angle);
    } else {
      Serial.printf("Invalid input: %d. Please enter 0–180.\n", angle);
    }
  }
}
