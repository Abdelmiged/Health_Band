#ifndef RangeChecker_h
#define RangeChecker_h

#include "Arduino.h"

class RangeChecker {
  public:
    RangeChecker(int id);
    String checker(float value, String toBeChecked);
  private:
    int ID;
    String checkTemperature(float value);
    String checkHumidity(float value);
    String checkHeartRate(float value);
    String checkOxygenLevel(float value);
};

#endif
