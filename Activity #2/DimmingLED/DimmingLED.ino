const int LED_PIN = 2;
const int POT_PIN = 34;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int potValue = analogRead(POT_PIN);         
  int dutyCycle = map(potValue, 0, 4095, 0, 255);

  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(dutyCycle * 10);  

  digitalWrite(LED_PIN, LOW);
  delayMicroseconds((255 - dutyCycle) * 10); 
}
