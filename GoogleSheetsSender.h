#ifndef GoogleSheetsSender_h
#define GoogleSheetsSender_h

#include "Arduino.h"
#include <HTTPClient.h>
#include "time.h"

class GoogleSheetsSender {
  public:
    GoogleSheetsSender(long gmtOffset, int daylightOffset, char* ntpserver);
    void SendToGoogleSheet(float temp, float humd, int heartRate, float oxygenLevel);
  private:
    char* ntpServer;
    long gmtOffset_sec;
    int daylightOffset_sec;
    String GOOGLE_SCRIPT_ID;
};

#endif
