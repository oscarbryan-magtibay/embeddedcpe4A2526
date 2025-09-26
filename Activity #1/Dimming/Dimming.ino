const int LED_PIN = 2;
const int POT_PIN = 4;

int potValue = 0;
int pwmValue = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  potValue = analogRead(POT_PIN);
  pwmValue = map(potValue, 0, 4095, 0, 255);

  analogWrite(LED_PIN, pwmValue);
  
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | PWM Value: ");
  Serial.println(pwmValue);

  delay(10);
}