#include <ESP32Servo.h>

Servo myServo;
int angle;

void setup() {
  Serial.begin(115200);
  myServo.attach(13);
  Serial.println("Enter angle (0-180):");
}

void loop() {
  if (Serial.available() > 0) {
    angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo set to: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid input. Enter 0-180.");
    }
  }
}
