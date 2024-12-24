#include "Arduino.h"
#include "RangeChecker.h"

RangeChecker::RangeChecker(int id) {
  ID = id;
}

String RangeChecker::checker(float value, String toBeChecked) {
  toBeChecked.toLowerCase();
  
  if(toBeChecked == "temperature")
    return checkTemperature(value);
  else if(toBeChecked == "humidity")
    return checkHumidity(value);
  else if(toBeChecked == "heartrate")
    return checkHeartRate(value);

  return checkOxygenLevel(value);
}

String RangeChecker::checkTemperature(float value) {
  if(value >= 20 && value <= 30)
    return "normal";
  else if(value > 30)
    return "above";

  return "below";
}

String RangeChecker::checkHumidity(float value) {
  if(value >= 55 && value <= 68)
    return "normal";
  else if(value > 68)
    return "above";

  return "below";
}

String RangeChecker::checkHeartRate(float value) {
  if(value >= 50 && value <= 100)
    return "normal";
  else if(value > 100)
    return "above";

  return "below";
}

String RangeChecker::checkOxygenLevel(float value) {
  if(value >= 96 && value <= 99)
    return "normal";
  else if(value > 99)
    return "above";

  return "below";
}
