const int ledPin = 2;     
const int potPin = 4;     
int potValue = 0;          
int pwmValue = 0;          

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