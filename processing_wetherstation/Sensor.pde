class Sensor {
  float previousValue;
  float currentValue;
  int lineColour;
  int maxValue;
  int minValue;
  String name;

  Sensor(String name, int min, int max, int colour) {
    this.name = name;
    maxValue = max;
    minValue = min;
    lineColour = colour;
    // for hacky testing only
    setCurrentValue(0);
  } 

  float getPreviousValue() { 
    return previousValue;
  }
  
  float getCurrentValue() { 
    return currentValue;
  }

  void setCurrentValue(float val) {
    previousValue = currentValue; 
    currentValue = val;
    // for hacky testing, only
    currentValue = minValue+((maxValue-minValue)/2);
    currentValue = (getMinValue() + (float)(Math.random() * (  (getMaxValue() - getMinValue())) + 1)) ;

  }
  
  int getMinValue() {return minValue;}
  int getMaxValue() {return maxValue;}
  int getLineColour() {return lineColour;}
  String getName(){return name;}
}

