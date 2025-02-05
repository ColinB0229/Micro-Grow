int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
int oldReading;
 
void setup(void) {
  Serial.begin(9600);   
}
 
void loop(void) {
  fsrReading = analogRead(fsrPin);  
  
  if(oldReading != fsrReading){

  Serial.print("Analog reading = ");
  Serial.print(fsrReading);     // print the raw analog reading


    if (fsrReading < 10) {
      Serial.println(" - Please refill :(");
    } else if (fsrReading < 200) {
      Serial.println(" - Refill soon :|");
    } else if (fsrReading < 500) {
      Serial.println(" - We pretty good :)");
    } else if (fsrReading < 800) {
      Serial.println(" - Thirst Quenched :)");
    } else {
      Serial.println(" - STOP!");
    }
  }
  oldReading = fsrReading;
  delay(1000);
}