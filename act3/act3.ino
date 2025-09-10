#include <ESP32Servo.h>

#define SERVO_PIN 23

Servo myServo;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  Serial.println("Enter 0-180:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int angle = input.toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Moved to ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid input.");
    }
  }
}
