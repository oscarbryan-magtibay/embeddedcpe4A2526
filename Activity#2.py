const int potPin = 34;  
const int ledPin = 13;   

// PWM settings
const int freq = 5000;   
const int ledChannel = 0;
const int resolution = 8; 

void setup() {
  // configure LED PWM
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
}

void loop() {
  int potValue = analogRead(potPin);        
  int dutyCycle = map(potValue, 0, 4095, 0, 255);  
  ledcWrite(ledChannel, dutyCycle);          
}
