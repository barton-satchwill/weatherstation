import processing.serial.*;
Sensor mySensor;

Serial myPort; 
int leftPad = 80;
int rightMargin = 20;
int x = leftPad;
int step = 2;

PFont font;

Sensor T = new Sensor("Temperature", -20, 30, #00008B);
Sensor P = new Sensor("Pressure", 500, 700, #F51F02);
Sensor A = new Sensor("Altitude", 500, 700, #02F54F);

void setup() {
  size(500, 200); 
  background(250); 

  font = createFont("Arial", 12);
  textAlign(RIGHT);
  textFont(font);
  fill(0);

  //hairlines();

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
  P.setCurrentValue(0);
  T.setCurrentValue(0);
  A.setCurrentValue(0);

  plot(P, (height/3)*0,10);
  plot(T, (height/3)*1,20);
  plot(A, (height/3)*2,30);
  x+=step;
  
  if (x >= width-rightMargin) {
    x = leftPad;
    background(g.backgroundColor);
    hairlines();
  }
}


void plot(Sensor s, int offset, int z) {
  int squeeze = 10;
  float bottom = offset+squeeze;
  float top = offset + height/3 - squeeze;
  float midLine = offset + height/6;

  if (x==leftPad) {
    fill(s.getLineColour());
    textAlign(RIGHT);
    textFont(font, 12);
    text(s.getName(), leftPad-5, midLine);
  }

  float y =  map(s.getCurrentValue(),  s.getMinValue(), s.getMaxValue(), bottom, top);
  float y1 = map(s.getPreviousValue(), s.getMinValue(), s.getMaxValue(), bottom, top);

  stroke(s.getLineColour());
  line(x, y1, x+step, y);
  line(leftPad, midLine, width, midLine);
}


//void serialEvent (Serial myPort) {
//}



boolean bStop;
void keyPressed()
{
  bStop = !bStop;
  if (bStop) {
    noLoop();
  }
  else {
    //background(g.backgroundColor);
    loop();
  }
}


void hairlines() {
  int colour = #FACFCF; 

  strokeWeight(1);
  textFont(font, 8);
  textAlign(LEFT);
  stroke(colour);
  fill(colour);
  int pad = 100;

  for (int x=20; x<= width-10; x=x+10) {
    if (x%50 == 0) text(x, x, 10);
    line(x+pad, 10, x+pad, height-10);
  }
  for (int y=10; y<=height-10; y=y+10) {
    if (y%50 == 0) text(y+"_", 10, y);
    line(20+pad, y, width-10, y);
  }
}

