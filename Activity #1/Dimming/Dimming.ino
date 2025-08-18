const int ledPin = 2;  // BACROYA_JOSEPH_W - DIMMING
const int freq = 5000;     
const int ledChannel = 0;  
const int resolution = 8;  

void setup() {
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(ledChannel, duty);
    delay(10);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(ledChannel, duty);
    delay(10);
  }
}
