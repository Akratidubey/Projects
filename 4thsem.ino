//This code is written by AKRATI DUBEY, University Roll No:- 2013241, Sectio :- B, Class Roll No:- 08
//Topic:- CLOUD BASED HUMIDITY AND TEMPERATURE MONITORING



#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin

#include <DHT.h>        

int Contrast=75;
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

#define DHTPIN 9                // Connect the signal pin of DHT11 sensor to digital pin 5
#define DHTTYPE DHT11   //can use dht11 also
DHT dht(DHTPIN, DHTTYPE);

String apiKey = "W0VG57ZZWVONK96J";     // replace with your channel's thingspeak WRITE API key

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
  
  
  String getStr = "GET /update?api_key=W0VG57ZZWVONK96J&field1=0";   // prepare GET string
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
//================================================================================ setup
void setup() {
  analogWrite(10,Contrast);   
  lcd.begin(16,2);             
  DEBUG=true;           // enable debug serial
  Serial.begin(9600); 
  
  dht.begin();          // Start DHT sensor
  
  espSerial.begin(115200);  // enable software serial
                         
     espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}


// ====================================================================== loop
void loop() {

  // Read sensor values
   float t = dht.readTemperature();
   float h = dht.readHumidity();
        if (isnan(t) || isnan(h)) {
        if (DEBUG) Serial.println("Failed to read from DHT");
      }
      else {
          if (DEBUG)  Serial.println("Temp="+String(t)+" *C");
          if (DEBUG) Serial.println("Humidity="+String(h)+" %");
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Humidity = ");
             lcd.print(h);
             lcd.setCursor(0,1);
             lcd.print("Temp = ");
             lcd.print(t);
           thingSpeakWrite(h,t);                                      // Write values to thingspeak
      }
  
    
  // thingspeak needs 15 sec delay between updates,     
  delay(20000);  
}
