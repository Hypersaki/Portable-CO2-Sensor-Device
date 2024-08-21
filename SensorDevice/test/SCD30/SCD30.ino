#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

SCD30 airSensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }
}

void loop() {
  if (airSensor.dataAvailable()) {
    Serial.print("CO2 (ppm): ");
    Serial.print(airSensor.getCO2());
    Serial.println();
  } else {
    Serial.println("Waiting for new data...");
  }

  delay(2000); // Wait 2 seconds between readings
}
