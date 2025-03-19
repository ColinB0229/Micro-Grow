#include <WiFiS3.h>  
#include "ArduinoGraphics.h" 
#include "Arduino_LED_Matrix.h" 
#include <WiFiUdp.h> 
#include <NTPClient.h> 
#include <Adafruit_NeoPixel.h>
#include <OneWire.h> //Temp Sensor 
#include <DallasTemperature.h>//Temp Sensor 

// WiFi Configuration
#define ssid "ME4195-wifi" 
#define pass "Electronics" 

// NeoPixel Configuration
#define LED_PIN 6        // Define LED pin
#define NUMPIXELS 24      // Change based on the number of LEDs
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int status = WL_IDLE_STATUS;

ArduinoLEDMatrix matrix; 
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP, "pool.ntp.org", -4 * 3600, 60000); // UTC-5 fixed offset
//Presure sensor variables 
int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
int oldReading = 0;
int revisedReading;
int i;
int sum;

//Temp Senosr /////////////////
// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


///Motor_and_Moist
int soilMoisture;
int BadLevel = 0;
int MehLevel = 1;
int GoodLevel = 2;
int Refill = 4;
int SoilLevel;

// Function Prototypes
void connectToWiFi(); 
void updateTime(); 
void displayTime(); 
void leddisplay(const char* text);
void LEDON();
void LEDOFF();
void Water_tank();
void temp_sensor();
void MotorControl();
int soilLevel(); 

void setup() {
    sensors.begin(); // Start up the library
    WiFi.begin(ssid, pass);
    matrix.begin(); //Matric Display 
    pixels.begin(); // Initialize NeoPixels
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);

    connectToWiFi();
    timeClient.begin();         
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
    static int lastDisplayedMinute = -1;  // Track last displayed time
    updateTime();
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    // Control LEDs based on time
    if ((hours == 9 && minutes >= 29) || (hours == 10 && minutes < 30)) {
        LEDON();
    } else {
        LEDOFF();
    }
    displayTime();
    //
    temp_sensor(); //check and display temp
    Water_tank(); //check water tank, think it should be moved to start up and pump function for V2.x
    
    
    MotorControl(analogRead(A2));
    
    /*
    // Check WiFi status
    if (WiFi.status() != WL_CONNECTED) {
        leddisplay("Connecting...");
        connectToWiFi();
    } else {
        static bool wifiDisplayed = false;
        if (!wifiDisplayed) {
            leddisplay("Connected!");
            delay(2000);  // Show "Connected!" for 2 seconds
            wifiDisplayed = true;  // Prevent showing it again
        }

        // Update time display every five mins
        if (minutes % 5 == 0 && minutes != lastDisplayedMinute) {
            displayTime();
            lastDisplayedMinute = minutes;  // Store last displayed minute
        }
    }
    */
}

// WiFi Connection Function
void connectToWiFi() {
    leddisplay("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {  
        WiFi.begin(ssid, pass);
        delay(5000);  // Retry every 5 seconds
    }
    leddisplay("Connected!");
}

// RTC Get Time from NTP Server
void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();  // Fetch latest time from NTP
    }
}

// RTC Convert and Display Time
void displayTime() {
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();

    char timeString[6]; // Adjusted to HH:MM
    sprintf(timeString, "%02d:%02d", hours, minutes);

    leddisplay(timeString);  
}

// LED Matrix Display Function
void leddisplay(const char* text) {
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textScrollSpeed(60);
    matrix.textFont(Font_5x7);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println(text);
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();
}

// LED Light Control Functions
void LEDON() {
    for(int i = 0; i < NUMPIXELS; i++) { 
        pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.show();
}

void LEDOFF() {
    for(int i = 0; i < NUMPIXELS; i++) { 
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
}

void Water_tank (){
  leddisplay("Tank Status:");
  fsrReading = analogRead(fsrPin);
  sum =0;
  for (i = 0; i < 10; i++) {
    sum += analogRead(fsrPin);
    delay(20);
  }
  oldReading = revisedReading;
  revisedReading = sum/10;
  
  
    if (revisedReading < 590) {
     leddisplay(" Refill");
    } else if (revisedReading < 685) {
      leddisplay(" Half-Full)");
    } else {
      leddisplay(" Full");
    }
  
  delay(1000);
}


void temp_sensor () {
// Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the busS
  sensors.requestTemperatures(); 
  char floating_text[10] ;

  dtostrf(sensors.getTempCByIndex(0), 6, 1, floating_text); //dtostrf(float_value, Min_width, Dig_afterDec, where to store)
  leddisplay("Soil Temp: ");
 leddisplay(floating_text);
 leddisplay("*C");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //leddisplay(sensors.getTempCByIndex(0)); 
}


void MotorControl(int soilMoisture){
  SoilLevel = soilLevel(soilMoisture);

  char soil_text[10] ;
  dtostrf(soilMoisture, 6, 1, soil_text);

  leddisplay("soil Moisture: ");
  leddisplay(soil_text);


  if(SoilLevel == BadLevel){
    leddisplay("Watering plant");
    digitalWrite(2, LOW);
    delay(5000);
    digitalWrite(2, HIGH);
    delay(10000);
  }
}

int soilLevel(int x){

    if(x < 200){
      delay(1000);
      if(x < 5){
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
