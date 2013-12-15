#include <Average.h>

// BMP085_test1
// by Filipe Vieira
// Simple test of BMP085 output using default settings.
// This example requires AUTO_UPDATE_TEMPERATURE to be true in bmp085.h otherwise temperature will not update.
// IMPORTANT!! To get correct values you MUST CHANGE init() parameters, in 
// this example I've set 250m based on GPS data for my location.

#include <Wire.h>
#include <BMP085.h>

#define CNT 60
int threshold = 2;
unsigned long p[CNT];
unsigned long a[CNT];

BMP085 dps = BMP085();      // Digital Pressure Sensor 

long Temperature = 0, Pressure = 0, Altitude = 0;
long sampleAlt = 0;

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);

  // uncomment for different initialization settings
  //dps.init();     // QFE (Field Elevation above ground level) is set to 0 meters.
  // same as init(MODE_STANDARD, 0, true);

  //  dps.init(MODE_STANDARD, 101417, false);  // 101850Pa = 1018.50hPa, false = using Pa units
  // this initialization is useful for normalizing pressure to specific datum.
  // OR setting current local hPa information from a weather station/local airport (QNH).

  dps.init(MODE_ULTRA_HIGHRES, 66800, true);  // 250 meters, true = using meter units
  // this initialization is useful if current altitude is known,
  // pressure will be calculated based on TruePressure and known altitude.

  // note: use zeroCal only after initialization.
  // dps.zeroCal(101800, 0);    // set zero point
  dps.getAltitude(&Altitude);
}            

// choose weight wetween 0..1
// new average = [sensor value * W] + [last average * (1-W)]
void loop(void) { 
  delay(1000);
  long alt;
  long avg;
  dps.getAltitude(&alt);
  avg = smooth(avg, alt, 0.1);
  Altitude = quantise(Altitude, alt, 50);
  Serial.println(Altitude-66800);
}



// new average = [sensor value * W] + [last average * (1-W)]
long smooth(long previousAverage, long sensorReading, double weight){
  long newAverage = ((sensorReading * weight) + (previousAverage *(1-weight)));
  return newAverage;
}

long quantise(long previous, long sensorReading, int threshold){
  int delta = abs(previous - sensorReading);
  if (delta > threshold){
//    Serial.print(previous); Serial.print(" - "); Serial.print(Altitude); Serial.print(" = "); Serial.println(delta);
    return sensorReading;
  }
  return previous;
}

