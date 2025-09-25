#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo bluetoothServo;

const int SERVO_PIN = 13;
const int STOP_PWM = 90;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo_Slave");

  bluetoothServo.attach(SERVO_PIN, 500, 2400);
  bluetoothServo.write(STOP_PWM);

  Serial.println("Slave is ready. Waiting for Master...");
}

void loop() {
  if (SerialBT.available()) {
    String data = SerialBT.readStringUntil('\n');
    data.trim();

    int input = data.toInt();

    if (input >= 0 && input <= 180) {
      Serial.printf("Input received: %d\n", input);

      int PWM_VALUE;

      if (input < 90) PWM_VALUE = map(input, 0, 89, 0, 89);
      else if (input > 90) PWM_VALUE = map(input, 91, 180, 91, 180);
      else PWM_VALUE = STOP_PWM;

      bluetoothServo.write(PWM_VALUE);

      delay(500);
      bluetoothServo.write(STOP_PWM);
    }
  }
}
