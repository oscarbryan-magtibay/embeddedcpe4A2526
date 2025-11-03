// Blinking LED using ESP32 (38-pin)
int ledPin = 2;  // Built-in LED pin or external LED on GPIO2

void setup() {
  pinMode(ledPin, OUTPUT);  // Set LED as output
}

void loop() {
  digitalWrite(ledPin, HIGH);  // Turn ON LED
  delay(1000);                 // Wait 1 second
  digitalWrite(ledPin, LOW);   // Turn OFF LED
  delay(1000);                 // Wait 1 second
}
