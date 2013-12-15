//**********************************************
// https://www.sparkfun.com/tutorials/253
//**********************************************

#include <Wire.h>

#define BMP085_ADDRESS 0x77  // I2C address of BMP085
#define BMP085_USE_DATASHEET_VALS 0
#define MODEL_SYNC 0

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// Use these for altitude conversions
const float p0 = 101325;     // average sea-level pressure (Pa)

// b5 is calculated in bmp085GetTemperature(...), 
// this variable is also used in bmp085GetPressure(...), so...
// Temperature(...) must be called before Pressure(...).
long b5; 

float temperature;
long pressure;
float altitude;

int sampleindex = 0;
const int samplesize = 20;
float samples[samplesize];
float sampletotal=0;
int cheezygraph_baseline = 668; // Edmonton elevation in meters
int loopcounter = 0;



void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up BMP085");
  Wire.begin();
  bmp085Calibration();

  for (int i = 0; i<samplesize; i++){
    samples[i]=0;
  }
  for (int i = 0; i<samplesize; i++){
    getSamples();
//    cheezygraph_baseline = altitude -  5;
  }
}



void loop()
{
  getSamples();  
  delay(500);
}



void getSamples()
{
  temperature = bmp085GetTemperature(bmp085ReadUT());
  pressure = bmp085GetPressure(bmp085ReadUP());
  altitude = bmp085GetAltitude(pressure);
 
  if (loopcounter++%20 ==0){
    Serial.println("\nTemperature (C)\tPressure (kPa)\tAltitude (m)");
  }
  Serial.print(temperature, 1);  Serial.print("\t\t");
  Serial.print(pressure/1000.0, 1);  Serial.print("\t\t");
//  cheezyGraph(altitude);  Serial.print("\t\t");
  Serial.print(altitude, 2); Serial.print("");
  Serial.println();
}



void cheezyGraph(float sample){
  for (float i=cheezygraph_baseline; i<sample; i=i+0.5){
    Serial.print("|");
  } 
}  



// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available()){ ; /* do nothing */  }
  data = Wire.read();
  return data;
}



// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2){ ; /* do nothing */  }
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}



// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void  bmp085Calibration() {
#if BMP085_USE_DATASHEET_VALS
  ac1 = 408;
  ac2 = -72;
  ac3 = -14383;
  ac4 = 32741;
  ac5 = 32757;
  ac6 = 23153;
  b1  = 6190;
  b2  = 4;
  mb  = -32768;
  mc  = -8711;
  md  = 2858;
#else
  ac1 = bmp085ReadInt(0xAA); 
  ac2 = bmp085ReadInt(0xAC); 
  ac3 = bmp085ReadInt(0xAE); 
  ac4 = bmp085ReadInt(0xB0); 
  ac5 = bmp085ReadInt(0xB2); 
  ac6 = bmp085ReadInt(0xB4); 
  b1 = bmp085ReadInt(0xB6);  
  b2 = bmp085ReadInt(0xB8);  
  mb = bmp085ReadInt(0xBA);  
  mc = bmp085ReadInt(0xBC);  
  md = bmp085ReadInt(0xBE); 
  
  // adjustments for this specific sensor
  md = 1882;
  ac1 = 6535;
#endif
  Serial.println("Reading Calibration Data");
  Serial.print("AC1: ");  Serial.println(ac1,DEC);  
  Serial.print("AC2: ");  Serial.println(ac2,DEC);  
  Serial.print("AC3: ");  Serial.println(ac3,DEC);  
  Serial.print("AC4: ");  Serial.println(ac4,DEC);  
  Serial.print("AC5: ");  Serial.println(ac5,DEC);  
  Serial.print("AC6: ");  Serial.println(ac6,DEC);  
  Serial.print("B1: ");  Serial.println(b1,DEC);  
  Serial.print("B2: ");  Serial.println(b2,DEC);  
  Serial.print("MB: ");  Serial.println(mb,DEC); 
  Serial.print("MC: ");  Serial.println(mc,DEC);  
  Serial.print("MD: ");  Serial.println(md,DEC);
}




// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
#if MODEL_SYNC
  Serial.print("\n\n\n\nut\t"); 
  Serial.println(ut);  
  Serial.print("\t"); 
  Serial.println();
#endif
return ut;
}



// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);

  // Wait for data to become available
  while(Wire.available() < 3){ ; /* do nothing */  }
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();

  up = (((unsigned long) msb << 16) | 
        ((unsigned long) lsb << 8) | 
        (unsigned long) xlsb) >> (8-OSS);

#if MODEL_SYNC
  Serial.print("msb\t"); Serial.println(msb);
  Serial.print("lsb\t"); Serial.println(lsb);
  Serial.print("xlsb\t"); Serial.println(xlsb);
  Serial.print("up\t"); Serial.println(up);
#endif
  return up;
}



// Calculate temperature given ut.
// Value returned will be in units of Celsius degrees
float bmp085GetTemperature(unsigned int ut)
{
  long x1, x2, t;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

#if MODEL_SYNC
  Serial.print("x1\t"); Serial.println(x1);
  Serial.print("x2\t"); Serial.println(x2);
  Serial.print("b5\t"); Serial.println(b5);
#endif

  t = ((b5 + 8)>>4);
#if MODEL_SYNC
  Serial.print("t\t"); Serial.println(t);
#endif

  return t/10.0;  
}



// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6>>12))>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;  

#if MODEL_SYNC
  Serial.print("\n");
  Serial.print("b6\t"); Serial.println(b6);
  Serial.print("x1\t"); Serial.println(x1);
  Serial.print("x2\t"); Serial.println(x2);
  Serial.print("x3\t"); Serial.println(x3);
  Serial.print("b3\t"); Serial.println(b3);
#endif

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * (b6 * b6>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

#if MODEL_SYNC
  Serial.print("\n");
  Serial.print("x1\t"); Serial.println(x1);
  Serial.print("x2\t"); Serial.println(x2);
  Serial.print("x3\t"); Serial.println(x3);
  Serial.print("b4\t"); Serial.println(b4);
#endif

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
#if MODEL_SYNC
  Serial.print("b7\t"); Serial.println(b7);
#endif

  if (b7 < 0x80000000) { 
    p = (b7*2)/b4;
//    Serial.print("<\np\t"); Serial.println(p);
  } else { 
    p = (b7/b4)*2;
//    Serial.print(">\np\t"); Serial.println(p);
  }

  x1 = (p>>8) * (p>>8);
#if MODEL_SYNC
  Serial.print("p\t"); Serial.println(p);
  Serial.print("x1\t"); Serial.println(x1);
#endif

  x1 = (x1 * 3038)>>16;
#if MODEL_SYNC
  Serial.print("x1\t"); Serial.println(x1);
#endif

  x2 = (-7357 * p)>>16;
#if MODEL_SYNC
  Serial.print("x2\t"); Serial.println(x2);
#endif

  p = p + ((x1 + x2 + 3791)>>4);
//  p = p + ((x1 + x2 + 3791 + 18570)>>4);
#if MODEL_SYNC
  Serial.print("p\t"); Serial.println(p);
#endif

// add mysterious adjustment
//   p = p + 774;
  return p;
}



float bmp085GetAltitude(long pressure){
  float alt = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
//  Serial.print(alt); Serial.print("-->");
  smooth(alt);
//  Serial.print(alt); Serial.print("-->");  
  if (abs(alt-altitude) > 0.25){
    altitude = alt;
  }
//  Serial.println(altitude);
  return altitude;
}



float smoothexponential(float data, float filterVal, float smoothedVal){
  if (filterVal > 1){      // check to make sure param's are within range
    filterVal = .99;
  }else if (filterVal <= 0){
    filterVal = 0;
  }
  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);
  return (int)smoothedVal;
}



void smooth(float &newValue) {
  sampletotal = sampletotal - samples[sampleindex];
  samples[sampleindex] = newValue;
  sampletotal = sampletotal + samples[sampleindex];
  sampleindex++;
  if (sampleindex >= samplesize){
    sampleindex = 0;
  }
  float average = sampletotal/samplesize;
//  Serial.print(newValue);
//  Serial.print("-->");
//  Serial.println(average);
  newValue = average;
}


