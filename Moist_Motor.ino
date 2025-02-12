int soilMoisture;
int BadLevel = 0;
int MehLevel = 1;
int GoodLevel = 2;
int Refill = 4;
int SoilLevel;

void setup(){

 Serial.begin(57600);
  pinMode(4, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(4, HIGH);
 soilMoisture = analogRead(A0);
 Serial.println(soilMoisture);
 delay(100);
  SoilLevel = soilLevel(soilMoisture);

if(SoilLevel == BadLevel){
  digitalWrite(4, LOW);
  delay(5000);
  digitalWrite(4, HIGH);
  delay(10000);
}

Serial.println(soilMoisture);

delay(100);

}

int soilLevel(int x){
if(x < 200){
  delay(1000);
  if(x < 50){
    return 4;
  }
  else{ 
    return 0;
  }
}
else if(x < 600){
  return 1;
}
else if(x < 1000){
  return 2;
}

}
