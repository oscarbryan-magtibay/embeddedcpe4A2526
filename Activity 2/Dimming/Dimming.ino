const int ledPin = 2;     
const int potPin = 34;     
int potValue = 0;          

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  potValue = analogRead(potPin);             
  int onTime = map(potValue, 0, 4095, 0, 1000); 
  int offTime = 1000 - onTime;                

  digitalWrite(ledPin, HIGH);               
  delayMicroseconds(onTime);                 
  digitalWrite(ledPin, LOW);               
  delayMicroseconds(offTime);                
  Serial.print("Potentiometer Value: ");
  Serial.println(potValue);
}
