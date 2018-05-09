float champ;
/* Arduino+esp8266 thingSpeak example  
 * Example name = "Write temperature and humidity to Thingspeak channel"
 * Created by Ilias Lamprou
 * Updated Oct 30 2016
 * 
 * Download latest Virtuino android app from the link:https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl
 * Video tutorial link: https://youtu.be/4XEe0HY0j6k
 * Contact address for questions or comments: iliaslampr@gmail.com
 */

// Code to use SoftwareSerial
#include <SoftwareSerial.h>
int x=0;
float val;
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

SoftwareSerial espSerial =  SoftwareSerial(4,5);  // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin
SoftwareSerial serialSIM800(7,8);


#include <DHT.h>        //Attention: For new  DHT11 version  library you will need the Adafruit_Sensor library
#include <Adafruit_Sensor.h>                        //Download from here: https://github.com/adafruit/Adafruit_Sensor
#include <MQ135.h>

const int a1=A1;
int FlamePin1 = A2;
int FlamePin2 = 13;

//Attention: For new  DHT11 version  libraries you will need the Adafruit_Sensor library                                  
//Download from here:https://github.com/adafruit/Adafruit_Sensor
//and install to Arduino software
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
 
 
float AccelMinX = 0;

float AccelMaxX = 0;
float AccelMinY = 0;
float AccelMaxY = 0;
float AccelMinZ = 0;
float AccelMaxZ = 0;

#include <MQ2.h>
  
  int pin = A0;
  
  //MQ2 mq2(pin);

#define DHTPIN 6                // Connect the signal pin of DHT11 sensor to digital pin 5
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);


String apiKey = "P6B4HL8SQTYEF4VD";     // replace with your channel's thingspeak WRITE API key

String ssid="Redmi";    // Wifi network SSID
String password ="shubham123";  // Wifi network password

boolean DEBUG=true;

//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value1, float value2, float value3, float value4){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr +="&field4=";
  getStr += String(value4);
 
  // ...
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}




//================================================================================ setup
void setup() {                
  DEBUG=true;
  // enable debug serial
  Serial.begin(9600); 
  serialSIM800.begin(9600);
  dht.begin();          // Start DHT sensor
 // mq2.begin();
  pinMode(FlamePin1, INPUT);
  pinMode(FlamePin2, INPUT);
pinMode( A1, INPUT); 
  pinMode(pin,INPUT);

  Serial.begin(9600);
  Serial.println("ADXL345 Accelerometer Calibration"); 
  Serial.println("");
  delay(100);

  sensors_event_t accelEvent;  
  accel.getEvent(&accelEvent);
  
champ = accelEvent.acceleration.x;
Serial.println(champ);
delay(1000);
  /*while(1)
  {
  sensors_event_t accelEvent;  
  accel.getEvent(&accelEvent);
  float val=accelEvent.acceleration.x;
  Serial.println(val);
  if (val<(champ-0.25)||val>(champ+0.25))
  {
    Serial.println("dfghgvfdxfcgvbvgfcdfcgvhbj");
    delay(200);
  }
  delay(200);
  }*/
  /* Initialise the sensor  */


   
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    // Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    // while(1);
  }
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(100);
  espSerial.begin(9600);  // enable software serial
                          // Your esp8266 module's speed is probably at 115200. 
                          // For this reason the first time set the speed to 115200 or to your esp8266 configured speed 
                          // and upload. Then change to 9600 and upload again
  
  //espSerial.println("AT+RST");         // Enable this line to reset the module;
  //showResponse(1000);

  //espSerial.println("AT+UART_CUR=9600,8,1,0,0");    // Enable this line to set esp8266 serial speed to 9600 bps
  //showResponse(1000);

  
  

  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}


// ====================================================================== loop
void loop() {
  while (Serial.available())
    {
      Serial.read();  // clear the input buffer
    }
    
  for (x=0;x<10;x++)
  {
    delay(1000);
    Serial.println("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");
    // Read sensor values
   float t = dht.readTemperature();
   float h = dht.readHumidity();
   

   float smoke = analogRead(pin);
   float sensorValue = analogRead(a1);
   int Flame1 = digitalRead(FlamePin1);
   int Flame2 = digitalRead(FlamePin2);

//Serial.println("Type key when ready..."); 
    //while (!Serial.available()){}  // wait for a character
    delay(200);
    /* Get a new sensor event */ 
    sensors_event_t accelEvent;  
    accel.getEvent(&accelEvent);
    float vib=accelEvent.acceleration.x;
  
   Serial.println(vib);





      
 if (Flame1==0)
 {
 
//Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("THERE IS A FIRE");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent! FLAME");
  }

  if (Flame2==0)
 {
 
//Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("THERE IS A FIRE");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent! FLAME");
  }
  

  if (t>50)
 {
 
//Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("TEMPERATURE IS HIGH");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent! TEMPERATURE");
  }


    if (h>50)
 {
 
//Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("HUMIDITY IS HIGH");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent! HUMIDITY");
  }

if (smoke>300)
 {
 
//Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("THERE MAY BE FIRE");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent! SMOKE");
  }

  if (vib<=(champ-0.25)|| vib>= (champ+0.25) )
  {
    //Set SMS format to ASCII
  serialSIM800.println("AT+CMGF=1\r\n");
  delay(100);
 
  //Send new SMS command and message number
  serialSIM800.println("AT+CMGS=\"+919404482860\"\r\n");
  delay(100);
   
  //Send SMS content
  serialSIM800.println("MAY BE EARTHQUAKE");
  delay(100);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.println((char)26);
  delay(100);
     
  Serial.println("SMS Sent!");
    }
  // thingspeak needs 15 sec delay between updates,       
    
  }


  Serial.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");    
   

        if (isnan(dht.readTemperature()) || isnan(dht.readHumidity())) {
        if (DEBUG) Serial.println("Failed to read from DHT and MQ2");
      }
      else {
          if (DEBUG)  Serial.println("Temp="+String(dht.readTemperature())+" *C");
          if (DEBUG) Serial.println("Humidity="+String(dht.readHumidity())+" %");
          if (DEBUG) Serial.println("CO2 in air ="+String(5*analogRead(A1)));
          if (DEBUG) Serial.println("Smoke="+String(analogRead(A0))+" %");
          if (DEBUG) Serial.println("Fire1="+String(digitalRead(A2))+" %");
          if (DEBUG) Serial.println("Fire2="+String(digitalRead(13))+" %");
         
           thingSpeakWrite(dht.readTemperature(),dht.readHumidity(),analogRead(A0),5*analogRead(A1));                                      // Write values to thingspeak
      }
  

x=0;
  
}




