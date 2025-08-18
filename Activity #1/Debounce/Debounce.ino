const int ledPin = 2;       
const int buttonPin = 4;     
int lastButtonState = HIGH;  
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  

void setup() {
  pinMode(ledPin, OUTPUT);       
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  int buttonState = digitalRead(buttonPin);
 
  if (buttonState == LOW && millis() - lastDebounceTime > debounceDelay) {
    lastDebounceTime = millis(); 
    digitalWrite(ledPin, !digitalRead(ledPin)); 
    
    while (digitalRead(buttonPin) == LOW);
  }
}
