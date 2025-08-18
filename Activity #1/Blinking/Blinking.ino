#define builtInLed 2

void setup() {
  pinMode(builtInLed, OUTPUT);  // Set pin as output
}

void loop() {
  digitalWrite(builtInLed, HIGH);  // LED ON
  delay(1000);                     
  digitalWrite(builtInLed, LOW);   // LED OFF
  delay(1000);                     
}
