#include <ESP32Servo.h>  // Include the ESP32 servo library

Servo myServo;           // Create a Servo object

int servoPin = 27;      // Pin connected to the servo
int angle = 0;          // Start at 0° (neutral position)
int newAngle = 0;       // Variable for new input angle

void setup() {
  Serial.begin(115200);        // Start serial communication
  myServo.attach(servoPin);    // Attach servo to pin 27
  myServo.write(angle);        // Set initial position
  Serial.println("Enter a value between 0 and 180 to move the servo:");
}

void loop() {
  if (Serial.available() > 0) {        // If data is available
    newAngle = Serial.parseInt();      // Read integer value

    // Only move if value is between 0 and 180
    if (newAngle >= 0 && newAngle <= 180) {
      angle = newAngle;                // Update angle
      myServo.write(angle);            // Move servo
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid input! Please enter 0 to 180 only.");
    }

    // Clear any leftover input (like newlines)
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}
