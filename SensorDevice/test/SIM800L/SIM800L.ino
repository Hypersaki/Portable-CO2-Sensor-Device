#include <SoftwareSerial.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2


void setup()
{
  delay(5000);
  
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing...");
 
  delay(5000);

  sendCommand("AT");

  sendCommand("AT+CSQ");
  
  sendCommand("AT+CCID");

  sendCommand("AT+CREG?");

  sendCommand("AT+CIPSHUT");
  
  delay(500);
  sendCommand("AT+SAPBR=0,1");
  delay(2000);
  
  sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");

  //Replace <your-apn> with your actual apn
  sendCommand("AT+SAPBR=3,1,\"APN\",\"everywhere\"");
  
}


void testPostRequest(){ 

  sendCommand("AT+SAPBR=1,1");

  sendCommand("AT+SAPBR=2,1");
  
  sendCommand("AT+HTTPINIT");

  sendCommand("AT+HTTPSSL=1");
  
  sendCommand("AT+HTTPPARA=\"CID\",1");

  //sendCommand("AT+HTTPPARA=\"URL\",\"adafruit.io feed key"");
  
  sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

  //You need to change 14 to the length of your data payload
  sendCommand("AT+HTTPDATA=14,10000");
  
  //Dummy data
  mySerial.println("{\"value\":25.5}");
  delay(5000);
  
  sendCommand("AT+HTTPACTION=1");
  delay(5000);
  
  sendCommand("AT+HTTPREAD");
  
  sendCommand("AT+HTTPTERM");

  sendCommand("AT+SAPBR=0,1");
  
}

void loop(){
  testPostRequest();
  delay(5000); 
}



void sendCommand(const char* command) {
  mySerial.println(command);
  delay(500);
  while (mySerial.available()) {
    char c = mySerial.read();
    Serial.write(c);
  }
  Serial.println("");
  delay(500);
}

