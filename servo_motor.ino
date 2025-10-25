#include <ESP32Servo.h>

Servo myServo;
int servoPin = 13;  // connect servo signal to GPIO13

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  Serial.println("Enter an angle (0 to 180):");
}

void loop() {
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    } else {
      Serial.println("Please enter a value between 0 and 180.");
    }
  }
}
