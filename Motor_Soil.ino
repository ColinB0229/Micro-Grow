int soilMoisture;
int BadLevel = 0;
int MehLevel = 1;
int GoodLevel = 2;
int newSoilLevel;
int oldSoilLevel = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(4, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  soilMoisture = analogRead(A0);
 oldSoilLevel = newSoilLevel;

if(soilMoisture < 200){
  newSoilLevel = BadLevel;
}
else if(soilMoisture < 600){
  newSoilLevel = MehLevel;
}
else if(soilMoisture < 1000){
  newSoilLevel = GoodLevel;
}

if(newSoilLevel != oldSoilLevel){
  Serial.println("The Soil Level Has Changed");
  if(newSoilLevel == 0){
    Serial.println("NEEDS WATER");
  }
  else if(newSoilLevel == 1){
    Serial.println("Solid Water Level");
  }
  else if(newSoilLevel == 2){
    Serial.println("Dirt Thirst is Quenched");
  }
}

Serial.println(soilMoisture);

delay(5000);
if(newSoilLevel == BadLevel){
  digitalWrite(4,  HIGH);
  delay(5000);
  digitalWrite(4,  LOW);
  delay(60000);
}



}
