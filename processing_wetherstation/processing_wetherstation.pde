import processing.serial.*;
Sensor mySensor;

Serial myPort;        // The serial port

int x = 0;
int y = 50;
int x1 = 0; 
float y1 = 0;
int H = 200;
int W = 400;

Sensor T = new Sensor("Temperature", -20, 30, #00008B);
Sensor P = new Sensor("Pressure", 500, 700, 0);

void setup() {
  size(500, 200); 
  
  // pg = createGraphics(100, 100);
  stroke(#00008B);
  

  println(Serial.list());
  String port;
  for (int i=0; i< Serial.list().length; i++) {
    if (Serial.list()[i] == "COM3") {
      port = "COM3";
    }
  }

  // don't generate a serialEvent() unless you get a newline character:
//  myPort.bufferUntil('\n');
}

void draw() { /* everything is done in serialEvent */
  x++;
  float y = (-20 + (float)(Math.random() * ((30 - -20) + 1)));
  T.setCurrentValue(y);
  y = map(y, T.getMinValue(), T.getMaxValue(), (height/2), 0)+ (height/2);
  y1 = map(T.getPreviousValue(), T.getMinValue(), T.getMaxValue(), (height/2), 0)+ (height/2);
  stroke(T.getLineColour());
  line(x, y1, x+5, y);

  y = (500 + (float)(Math.random() * ((700 - 500) + 1)));
  P.setCurrentValue(y);
  y = map(y, P.getMinValue(), P.getMaxValue(), (height/2), 0);
  y1 = map(P.getPreviousValue(), P.getMinValue(), P.getMaxValue(), (height/2), 0);
//  y = map(y, 0, 200, height, 0);
//  y1 = map(P.getPreviousValue(), 0, 200, height, 0);
  println(y1+" --> " + y);
  stroke(P.getLineColour());
  line(x, y1, x+5, y);
  
  x+=5;
 
  if (x >= width) {
    x = 0;
    background(g.backgroundColor);
    float referenceLine = map(0, -20, 30, height, 0); 
    line(0, referenceLine, width, referenceLine);
  }
}


//void serialEvent (Serial myPort) {
//  x1 = x;
//  y1 = y;
//  x++;
//  
//
////  y = (int)getMappedValue(t, -20, 30);
//  line(x1, y1, x, y);
//
//  // at the edge of the screen, go back to the beginning:
//  if (x >= width) {
//    x = 0;
//    background(g.backgroundColor);
//    float referenceLine = map(0, -20, 30, height, 0); 
//    line(0, referenceLine, width, referenceLine);
//  }
//}
//
//
//
//
//
//
//float readValue() {
//  float inByte = 0;
//  String inString = myPort.readString();
//  if (inString != null) {
//    inString = trim(inString);
//    print(inString); 
//    print("-->");
//    inByte = float(inString); 
//    print(inByte);
//  }
//  return inByte;
//}
//
//float getMappedValue(float value, int min, int max) {
//  println("  mapping " + value);
//  return map(value, min, max, height, 0);
//}
//
//
//String[] parseString(String sentence){
//  String[] x = sentence.split(" ");
//  println(x[1]);
//  return x;
//}
//

boolean bStop;
void keyPressed()
{
  bStop = !bStop;
  if (bStop) {
    noLoop();
  }
  else {
    background(g.backgroundColor);
    loop();
  }
}

