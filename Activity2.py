 const int ledPin = 2;       
const int potPin = 34;     

int potValue = 0;           
int pwmValue = 0;           

const int freq = 5000;      
const int ledChannel = 0;   
const int resolution = 8;   

void setup() {  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  Serial.begin(115200);
}

void loop() {  
  potValue = analogRead(potPin);
  pwmValue = map(potValue, 0, 4095, 0, 255);
  ledcWrite(ledChannel, pwmValue);
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | PWM Value: ");
  Serial.println(pwmValue);
  delay(10);  
}