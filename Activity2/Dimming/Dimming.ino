#define led 2
#define potentiometer 23
int dimControl = 0;
void setup(){
  pinMode(led,OUTPUT);
  pinMode(potentiometer,INPUT);

}
void loop(){

dimControl = analogRead(potentiometer);

int Brightness = map(dimControl,0,4095,0,255 );

analogWrite(led,Brightness);



}