#include <ESP32Servo.h>

Servo servo;  

const int SERVO_PIN = 22;  
int currentAngle = 0;

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  Serial.println("Enter angle (0 - 180):");
}

void loop() {
  if (!Serial.available()) return;  

  int newAngle = Serial.parseInt();  

  if (newAngle < 0 || newAngle > 180) {
    Serial.println("Invalid input. Try 0 - 180.");
    return;
  }

  currentAngle = newAngle;
  servo.write(currentAngle);
  Serial.printf("Servo set to: %d\n", currentAngle);
}
