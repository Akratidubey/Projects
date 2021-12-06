//This code is written by ANAND KUMAR YADAV for HALL EFFECT WATER SENSOR to measure the flow rate in liters per minute and total volume in liters and upload the data to cloud server(IOT) 
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;
const int sensor = A0;
const int LED = 13;

#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // REMEMBER arduino RX pin=2, arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin and connect the arduino TX pin to esp8266 module RX pin

String apiKey = "TQ6AVWHA3VXLWVV8";     // replace your WRITE API Key of the thingspeak Server

String ssid="Akrati";    // repace with your own wifi network id
String password ="akratiakku";  // Wifi network password (try to keep the wifi password in letters only. Avoid any digits and special symbols)

boolean DEBUG=true;

//=============================================================================== Response Function for ESP 8266-01 Module

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

//================================================================================
boolean thingSpeakWrite(float value1, float value2){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=TQ6AVWHA3VXLWVV8&field1=0";   // prepare GET string (update your own thingspeak server URL for writing the data to the cloud server)
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  // getStr +="&field3=";
  // getStr += String(value3);
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
//============================================================================Setup function


void setup()
{
pinMode(LED,OUTPUT);
DEBUG=true; 
Serial.begin(9600);
Serial.println("Water Flow Meter");
Serial.println("****************");
delay(2000);
pinMode(sensor,INPUT);
espSerial.begin(115200); //start the esp8266 sensor module

espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}
void loop()
{
  digitalWrite(LED,HIGH);
X = pulseIn(sensor, HIGH);
Y = pulseIn(sensor, LOW);
TIME = X + Y;
FREQUENCY = 1000000/TIME;
WATER = FREQUENCY/7.5;
LS = WATER/60;
if(FREQUENCY >= 0)
{
if(isinf(FREQUENCY))
{
Serial.println("Flow Rate: 0.00 L/M");
Serial.print("Volume:");
Serial.print( TOTAL);
Serial.println(" L");
}
else
{
TOTAL = TOTAL + LS;
Serial.print("Flow Rate: ");
Serial.print(WATER);
Serial.println(" L/M");
Serial.print("Volume : ");
Serial.print( TOTAL);
Serial.println(" L");
thingSpeakWrite(WATER,TOTAL);
}
}
delay(1000);
}
