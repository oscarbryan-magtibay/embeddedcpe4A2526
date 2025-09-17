#Mac Address

#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Get and print the ESP32's WiFi MAC address
  String mac = WiFi.macAddress();
  Serial.print("ESP32 MAC Address: ");
  Serial.println(mac);
}

void loop() {
  // Nothing needed here
}

#Master

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

String slaveMac = "24:6F:28:AA:BB:CC";  // Replace with Slave ESP32 MAC
int angle = 0;

void setup() {
  Serial.begin(115200);

  if (!SerialBT.begin("ESP32_Master", true)) { // true = Master mode
    Serial.println("Failed to start Bluetooth!");
    while (1);
  }

  Serial.println("Connecting to Slave via MAC...");
  if (SerialBT.connect(slaveMac)) {
    Serial.println("Connected to Slave!");
  } else {
    Serial.println("Failed to connect.");
  }
}

void loop() {
  if (Serial.available()) {
    angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      SerialBT.println(angle);  
      Serial.print("Sent angle: ");
      Serial.println(angle);
    } else {
      Serial.println("Enter valid angle (0–180).");
    }
  }
}


#Slave

#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;
int angle = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Slave", false);  // false = Slave mode
  myServo.attach(13);  // Servo pin
  Serial.println("Slave ready, waiting for Master...");
}

void loop() {
  if (SerialBT.available()) {
    angle = SerialBT.parseInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    }
  }
}
