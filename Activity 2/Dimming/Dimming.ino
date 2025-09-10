const int ledPin = 2;      // LED pin
const int potPin = 34;     // Potentiometer pin
int potValue = 0;          // Variable to store potentiometer value
int dutyCycle = 0;        

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {

  potValue = analogRead(potPin);
  dutyCycle = map(potValue, 0, 4095, 0, 100);

  int period = 1000;
  int onTime = (dutyCycle * period) / 100;
  int offTime = period - onTime;

  if (onTime > 0) {
    digitalWrite(ledPin, HIGH);
    delayMicroseconds(onTime);
  }
  if (offTime > 0) {
    digitalWrite(ledPin, LOW);
    delayMicroseconds(offTime);
  }


  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.println("%");
}
