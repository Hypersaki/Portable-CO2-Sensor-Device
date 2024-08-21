#include <Wire.h>
#include <TinyGPS++.h>
#include "SparkFun_SCD30_Arduino_Library.h"

// Use hardware serial for communication with SIM800L and GPS
#define SIM800LSerial Serial1
#define GPSSerial Serial3

TinyGPSPlus gps;
SCD30 airSensor;

uint16_t CO2DATA = 0;
float lon = 0;
float lat = 0;
char CO2Char[30];
char LatChar[30];
char LonChar[30];
char buffer[100];

void setup() {
  delay(5000);
  Wire.begin();
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  // Begin serial communication with Arduino and SIM800L
  SIM800LSerial.begin(9600);
  // Begin serial communication with Arduino and GPS module
  GPSSerial.begin(9600);
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }

  Serial.println("Initializing...");
  delay(1000);
  sendCommand("AT");
  sendCommand("AT+CSQ");
  sendCommand("AT+CCID");
  sendCommand("AT+CREG?");
  sendCommand("AT+CIPSHUT");

  delay(500);
  sendCommand("AT+SAPBR=0,1");
  delay(1000);
  sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  sendCommand("AT+SAPBR=3,1,\"APN\",\"everywhere\"");
  Serial.println("Setup Complete");
}

void PostRequest() {
  sendCommand("AT+SAPBR=1,1");
  sendCommand("AT+SAPBR=2,1");
  sendCommand("AT+HTTPINIT");
  sendCommand("AT+HTTPSSL=1");
  sendCommand("AT+HTTPPARA=\"CID\",1");
  //sendCommand("AT+HTTPPARA=\"URL\",\"adafruit.io feed"");
  sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

  // Update payload length
  handleBuffer();
  int payloadLength = strlen(buffer);
  char command[30];
  sprintf(command, "AT+HTTPDATA=%d,10000", payloadLength);
  sendCommand(command);

  // Send the data
  SIM800LSerial.print(buffer);
  delay(2000);

  sendCommand("AT+HTTPACTION=1");
  delay(2000);
  sendCommand("AT+HTTPREAD");
  sendCommand("AT+HTTPTERM");
  sendCommand("AT+SAPBR=0,1");
}

void loop() {
  readCO2();
  readgps();
  PostRequest();
  delay(1000);
}

void readgps() {
  // Use a timeout mechanism to avoid infinite loops
  unsigned long start = millis();
  const unsigned long timeout = 2000;  // 2 seconds timeout

  while (millis() - start < timeout) {
    if (GPSSerial.available() > 0) {
      gps.encode(GPSSerial.read());
      if (gps.location.isValid()) {
        lat = gps.location.lat();
        lon = gps.location.lng();
        Serial.print(lat, 6);
        Serial.print(F(","));
        Serial.println(lon, 6);
        char floatStr[20];
        Serial.println("test");
        
        dtostrf(lat, 6, 6, floatStr);
        Serial.println(floatStr);
        sprintf(LatChar, "\"lat\":%s,", floatStr);
        Serial.println(LatChar);
        dtostrf(lon, 6, 6, floatStr);
        Serial.println(floatStr);
        sprintf(LonChar, "\"lon\":%s}", floatStr);
        break;
      } else {
        Serial.print(F("INVALID"));
      }
    }
  }
}

void readCO2() {
  if (airSensor.dataAvailable()) {
    CO2DATA = airSensor.getCO2();
    Serial.print("CO2 (ppm): ");
    Serial.print(CO2DATA);
    Serial.println();
    sprintf(CO2Char, "{\"value\":%d,", CO2DATA);
  } else {
    Serial.println("Waiting for new CO2 data...");
    sprintf(CO2Char, "{\"CO2(ppm)\":0,");
  }
}

void handleBuffer() {
  Serial.println("123");//test this function working
  Serial.println(LatChar);
  Serial.println(LonChar);
  sprintf(buffer, "%s%s%s", CO2Char, LatChar, LonChar);
  Serial.println(buffer);
}

void sendCommand(const char* command) {
  SIM800LSerial.println(command);
  delay(500);
  while (SIM800LSerial.available()) {
    char c = SIM800LSerial.read();
    Serial.write(c);
  }
  Serial.println("");
  delay(500);
}
