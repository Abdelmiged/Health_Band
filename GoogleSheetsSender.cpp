#include "Arduino.h"
#include "GoogleSheetsSender.h"

GoogleSheetsSender::GoogleSheetsSender(long gmtOffset, int daylightOffset, char* ntpserver) {
  gmtOffset_sec = gmtOffset;
  daylightOffset_sec = daylightOffset;
  ntpServer = ntpserver;
  GOOGLE_SCRIPT_ID = ""; // Google Script ID that handles data received from ESP32 and inserts them into the recipient's Google sheet
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void GoogleSheetsSender::SendToGoogleSheet(float temp, float humd, int heartRate, float oxygenLevel) {
//  delay(3000);
  String tempString = String(temp);
  String humdString = String(humd);
  String heartRateString = String(heartRate);
  String oxygenLevelString = String(oxygenLevel);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  String asString(timeStringBuff);
  asString.replace(" ", "-");
  String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString + "&temperature=" + tempString + "&humditiy=" + humdString + "&heartrate=" + heartRateString + "&oxygenlevel=" + oxygenLevelString;
  HTTPClient http;
  http.begin(urlFinal.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  http.end();
  delay(1500);
}
