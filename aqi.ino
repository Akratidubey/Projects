//This code is written by AKRATI DUBEY, University Roll No:- 2013241, Sectio :- B, Class Roll No:- 08
//Topic:- CLOUD BASED AIR QUALITY INDEX MONITORING SYSTEM

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin

#define beeper 8                       //beeper on Digital 8
#define sensor A0                      //sensor on Analog 0
#define LED 9

int Contrast=15;
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

String apiKey = "MRGJ7U28IPM5LM5D";     // replace with your channel's thingspeak WRITE API key

String ssid="vivo";    // Wifi network SSID
String password ="amazon12";  // Wifi network password

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
boolean thingSpeakWrite(float value1){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=MRGJ7U28IPM5LM5D&field1=0";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  
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
  analogWrite(10,Contrast);
  Serial.begin(9600);                     //start serial comms
  pinMode(beeper,OUTPUT);                 //set beeper for output
  pinMode(sensor,INPUT);                  //set sensor for input
  pinMode(LED,OUTPUT);
  
  lcd.begin(16, 2);                       //initial LCD setup
  lcd.setCursor (0,0);                    //splash screen and warmup
  lcd.print("                ");
  lcd.setCursor (0,1);
  lcd.print("                ");
  lcd.setCursor (0,0);
  lcd.print("   Air Sensor   ");
  lcd.setCursor (0,1);
  lcd.print("   Warming Up   ");
  delay(2000);                            //set for at least 2 minutes

  lcd.setCursor (0,0);                    //clear screen
  lcd.print("                ");
  lcd.setCursor (0,1);
  lcd.print("                ");

espSerial.begin(115200);  // enable software serial
                         
     espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");  
}

void loop() {
  
float air_quality = analogRead(A0);
if(air_quality>=400)
{digitalWrite(9,HIGH); 
digitalWrite(8,LOW);
}
else 
{digitalWrite(9,LOW); 
digitalWrite(8,HIGH);
}
lcd.setCursor (0,0);
lcd.print("Air Quality is:");
lcd.setCursor(0,1);
lcd.print(air_quality);
thingSpeakWrite(air_quality);
// thingspeak needs 15 sec delay between updates,     
}
