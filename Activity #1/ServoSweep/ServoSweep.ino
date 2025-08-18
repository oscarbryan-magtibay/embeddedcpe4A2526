#include <Servo.h>

const int potPin = 34;     // Potentiometer pin (analog input, ESP32)
const int servoPin = 13;   // Servo control pin

Servo myServo;             // Create Servo object

void setup() {
  myServo.attach(servoPin);   // Attach servo to pin
  Serial.begin(115200);       // Start serial communication
}

void loop() {
  // Read potentiometer value (0–4095 on ESP32)
  int potValue = analogRead(potPin);

  // Map potentiometer value to servo angle (0–180°)
  int servoAngle = map(potValue, 0, 4095, 0, 180);

  // Move servo to the mapped angle
  myServo.write(servoAngle);

  // Print values for debugging
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | Servo Angle: ");
  Serial.println(servoAngle);

  delay(15); // Small delay for smooth servo movement
}
