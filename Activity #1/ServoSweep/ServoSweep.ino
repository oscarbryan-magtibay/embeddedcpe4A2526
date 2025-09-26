#include <ESP32Servo.h>

const int POT_PIN = 34;
const int SERVO_PIN = 2;

Servo myServo;

void setup() {
  myServo.attach(SERVO_PIN);
  Serial.begin(115200);
}

void loop() {
  int potValue = analogRead(POT_PIN);
  int servoAngle = map(potValue, 0, 4095, 0, 100);

  myServo.write(servoAngle);

  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print("Servo Angle: ");
  Serial.print(servoAngle);

  delay(15);
}