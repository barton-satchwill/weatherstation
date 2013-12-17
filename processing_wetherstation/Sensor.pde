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
  }
  
  int getMinValue() {return minValue;}
  int getMaxValue() {return maxValue;}
  int getLineColour() {return lineColour;}
}

