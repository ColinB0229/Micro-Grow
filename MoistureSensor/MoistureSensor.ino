void setup(){

 Serial.begin(57600);

}
void loop(){

 Serial.print("Moisture Sensor Value: ");
 Serial.println(analogRead(A0));
 delay(100);

}