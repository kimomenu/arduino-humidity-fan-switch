#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_LiquidCrystal.h"
#include <Adafruit_AHTX0.h>

///buttons initializing
const int relayPin = 2;  // the number of the pushbutton pin
const int fanPin = 5;  // the number of the pushbutton pin
const int upbuttonPin = 7;  // the number of the pushbutton pin
const int downbuttonPin = 6;  // the number of the pushbutton pin

int upbuttonState = 0;  // variable for reading the pushbutton status
int downbuttonState = 0;  // variable for reading the pushbutton status
int hum_setpoint = 50;  // variable counting button pulses up and down as setpoint
int mult = 5; // multiplier for setpoint
int timer = 0; // timer variable to count seconds between start and end
const int relay_delay = 30; // time between a start and a stop in cycles


//LCD initi
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

//// HUMIDITY SENSOR init
Adafruit_AHTX0 aht;


// A simple data logger for the Arduino analog pins
// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  2000 // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 2000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()
uint32_t delayMS;

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define redLEDpin 3
#define greenLEDpin 4

// The analog pins that connect to the sensors
#define BANDGAPREF 14            // special indicator that we want to measure the bandgap
#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
#define bandgap_voltage 1.1      // this is not super guaranteed but its not -too- off

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;




void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}



void setup(void)
{
  
  Serial.begin(9600);

  // use for AHT20
  Serial.println("Adafruit AHT10/AHT20 demo!");

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  Serial.println();
  
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  // initialize the up down button pins as an input:
  pinMode(upbuttonState, INPUT);
  pinMode(downbuttonState, INPUT);
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
  
  logfile.println("millis,stamp,datetime,temp,humidity,voltage,fan");    
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,datetime,temp,humidity,voltage,fan");
#endif //ECHO_TO_SERIAL
 
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);

  // lcd startup
  lcd.begin(16, 2);
}



void loop(void)
{
  
  // logging data from AHT temp and humidity sensor
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  float tempReading=temp.temperature;
  float humidityReading=humidity.relative_humidity;

  // SD CARD PART ///
  digitalWrite(greenLEDpin, HIGH);
  
  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(", ");    
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.unixtime()); // seconds since 1/1/1970
  logfile.print(", ");
  logfile.print('"');
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL
 
  logfile.print(", ");    
  logfile.print(tempReading);
  logfile.print(", ");    
  logfile.print(humidityReading);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(tempReading);
  Serial.print(", ");    
  Serial.print(humidityReading);
  Serial.print(", ");    
  Serial.print(timer);
#endif //ECHO_TO_SERIAL

  // Log the estimated 'VCC' voltage by measuring the internal 1.1v ref
  analogRead(BANDGAPREF); 
  delay(10);
  int refReading = analogRead(BANDGAPREF); 
  float supplyvoltage = (bandgap_voltage * 1024) / refReading; 
  
  logfile.print(", ");
  logfile.print(supplyvoltage);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(supplyvoltage);
#endif // ECHO_TO_SERIAL

  digitalWrite(greenLEDpin, LOW);

  upbuttonState = digitalRead(upbuttonPin);
  downbuttonState = digitalRead(downbuttonPin);

  
  // lcd configuration
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  lcd.print("T");
  lcd.print(tempReading);
  lcd.print("C");
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print("  H");
  lcd.print(humidityReading);
  lcd.print("%");  

  // lcd configuration
  lcd.setCursor(0, 1);
  /*lcd.print("U ");
  lcd.print(upbuttonState);

  lcd.print(" D ");
  lcd.print(downbuttonState);*/
  lcd.print("Setpoint H");

  hum_setpoint = hum_setpoint +  upbuttonState*mult - downbuttonState*mult;


  lcd.print(hum_setpoint);
  lcd.print(" %          ");

  timer++; // adds one value to timer
  

  if (hum_setpoint<humidityReading && timer>relay_delay) { // turn on led 
    digitalWrite(fanPin, HIGH);
    digitalWrite(relayPin, HIGH);
    logfile.print(", fan on ");
    
    timer=0;
    
  }
  else if(hum_setpoint>=humidityReading && timer>relay_delay) { //turn off led
    digitalWrite(fanPin, LOW);
    digitalWrite(relayPin, LOW);
    logfile.print(", fan off ");
    timer=0   ;
  }

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  digitalWrite(redLEDpin, LOW);

  delay(2000);

  lcd.setCursor(0, 1);
  lcd.print("              ");

  // lcd configuration
  lcd.setCursor(0, 1);
  //lcd.print("Testttt            ");
  lcd.print(now.year(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  lcd.print(" ");
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.setBacklight(HIGH);
  
}
