// Servo Sweep using ESP32
#include <ESP32Servo.h>

Servo myservo;
int pos = 0;

void setup() {
  myservo.attach(13);  // Attach servo signal pin to GPIO13
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) {  // Move from 0° to 180°
    myservo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {  // Move back from 180° to 0°
    myservo.write(pos);
    delay(15);
  }
}
