 #define  LED_PIN 2

 void setuo=p() {
   pinmode(LED_PIN, OUTPUT);
 }
 void loop(){
  digitalWrite (LED_PIN, HIGH);
  delay(1000);
  digtalWrite(LED_PIN, LOW);
  delay(1000);
 }
