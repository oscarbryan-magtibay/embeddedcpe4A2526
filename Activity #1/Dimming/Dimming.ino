const int ledPin = 2;     
const int potPin = 4;     
int potValue = 0;          
int PwmValue = 0;          

void setup() {
  
  pinMode(ledPin, OUTPUT);

  
  Serial.begin(115200);
}

void loop() {
  potValue = analogRead(potPin);

  PwmValue = map(potValue, 0, 4095, 0, 255);

  analogWrite(ledPin, PwmValue);

  Serial.print(potValue);
  Serial.println(PwmValue);

  delay(10);
}