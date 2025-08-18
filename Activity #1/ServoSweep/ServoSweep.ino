#include <ESP32Servo.h>

Servo myServo;
int potPin = 12;
int potValue = 0;
int angle = 0;

void setup() {
  myServo.attach(14);
}

void loop() {
  potValue = analogRead(potPin);
  angle = map(potValue, 0, 4095, 0, 180);
  myServo.write(angle);
  delay(15);
}