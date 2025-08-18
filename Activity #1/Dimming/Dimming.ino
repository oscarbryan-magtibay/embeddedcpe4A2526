const int ledPin = 2;      // LED pin
const int potPin = 34;     // Potentiometer pin (analog input)
int potValue = 0;          // Variable to store potentiometer value
int pwmValue = 0;          // PWM value to control LED brightness

void setup() {

  pinMode(ledPin, OUTPUT);


  Serial.begin(115200);
}

void loop() {

  potValue = analogRead(potPin);


  pwmValue = map(potValue, 0, 4095, 0, 255);

  analogWrite(ledPin, pwmValue);

  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | PWM Value: ");
  Serial.println(pwmValue);

  delay(10);
}
