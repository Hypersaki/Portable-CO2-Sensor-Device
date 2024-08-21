#include <SoftwareSerial.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;
SoftwareSerial ss(5,4);

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("Setup Complete");
}

void loop()
{
  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated())
    {
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6); 
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }
  }
}
