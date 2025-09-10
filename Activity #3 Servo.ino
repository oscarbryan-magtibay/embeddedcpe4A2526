#include <ESP32Servo.h>

Servo myServo;  
int servoPin = 18;  // You can change this to any valid PWM GPIO pin on ESP32
int angle = 0;
  
void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  Serial.println("Type an angle between 0 and 180:");
}

void loop() {
  if (Serial.available() > 0) {
    int inputAngle = Serial.parseInt();  // Read integer from Serial
    if (inputAngle >= 0 && inputAngle <= 180) {
      angle = inputAngle;
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid angle! Please enter 0 to 180.");
    }
  }
}
