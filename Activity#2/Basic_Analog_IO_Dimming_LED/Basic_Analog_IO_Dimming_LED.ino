const int potPin = 34;   // Potentiometer analog input (0–4095)
const int ledPin = 27;   // LED digital output
unsigned long previousMicros = 0;

int dutyCycle = 0;       // Duty cycle (0–100)
int pwmPeriod = 1000;    // 1 kHz simulated PWM
int pwmOnTime = 0;       // Time LED stays ON (microseconds)

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Basic Analog IO – Smooth Dimming LED using Potentiometer");
}

void loop() {
  // Read potentiometer and map to 0–100 %
  int potValue = analogRead(potPin);
  dutyCycle = map(potValue, 0, 4095, 0, 100);
  pwmOnTime = (pwmPeriod * dutyCycle) / 100;

  // Non-blocking PWM generation
  unsigned long now = micros();
  unsigned long cyclePos = now % pwmPeriod;

  if (cyclePos < pwmOnTime)
    digitalWrite(ledPin, HIGH);
  else
    digitalWrite(ledPin, LOW);

  // Optional serial monitor display
  static unsigned long lastPrint = 0;
  if (now - lastPrint >= 50000) { // every 50 ms
    lastPrint = now;
    Serial.print("Potentiometer: ");
    Serial.print(potValue);
    Serial.print(" | DutyCycle: ");
    Serial.print(dutyCycle);
    Serial.println("%");
  }
}
