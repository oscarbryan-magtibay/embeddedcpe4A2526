int potPin = 36;   // Potentiometer middle pin to GPIO36 (ADC1_CH0)
int ledPin = 2;    // LED on GPIO2

int pwmChannel = 0;
int freq = 5000;
int resolution = 8; // 8-bit resolution (0-255)

void setup() {
  Serial.begin(115200);
  
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(ledPin, pwmChannel);
}

void loop() {
  int potValue = analogRead(potPin); // 0-4095
  int dutyCycle = map(potValue, 0, 4095, 0, 255);
  
  ledcWrite(pwmChannel, dutyCycle);
  
  delay(10);
}
