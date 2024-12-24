#ifndef HTMLDocument_h
#define HTMLDocument_h

#include "Arduino.h"
#include "RangeChecker.h"
#include <WiFi.h>

class HTMLDocument {
  public:
    HTMLDocument(WiFiClient client);
    void HTMLDocumentResponse(float temperatureReading, float humidityReading, int heartRateReading, float oxygenlevelReading);
  private:
    void HTTPResponseHeaders();
    String HTMLStyle();
    String setSpanDesign(String postFix, float value, String toBeChecked);
    WiFiClient _client;
};

#endif
