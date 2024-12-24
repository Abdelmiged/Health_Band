#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include "HTMLDocument.h"
#include "EmailSender.h"
#include "GoogleSheetsSender.h"
#include "RangeChecker.h"
#include <ESP32Servo.h>

Servo myservo;
RangeChecker checker(1);

TaskHandle_t Task1;

MAX30105 particleSensor;

const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 

float tempReading;
float humdReading;

float beatsPerMinute;
int beatAvg;
int oxygenLvl;

#define DHTPin 15
#define DHTType DHT11
// WiFi SSID and Password for ESP32 to be able to connect to a network
const char* ssid = "";
const char* password = "";

String recipientEmail = ""; // Put the email of the user who is receiving data into his Google Spreadsheet

// Third parameter is for an email made for the ESP32 so it can send an email to the recipient
EmailSender emailSender("smtp.gmail.com", 465, "", "kmuy kack ngce zhmn", recipientEmail);

unsigned long sendToSheetDelay = 0;
unsigned long sendEmailDelay = 0;
bool sendEmail = false;

WiFiServer server(80);
DHT dht(DHTPin, DHTType);

void checkConnection() {
  delay(500);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  delay(500);
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.print(WiFi.localIP());
  server.begin();
}

String getPreferredEmail(String httpRequestLine) {
  bool stopFetching = false;
  String email = "";
  for(int i = 0; i < httpRequestLine.length(); i++) {
    if(httpRequestLine[i] == '/' and stopFetching == false) {
      stopFetching = true;
      continue;
    }

    if(stopFetching == true) {
      if(httpRequestLine[i] == ' ')
        break;
      email += httpRequestLine[i];
    }
  }

  if(email == "favicon.ico")
    email = "";
  return email;
}

void clientConnection() {
  WiFiClient client = server.available();

  if(client) {
    String currentHTTPRequestLine = "";
    bool firstRequestLine = true;
    while(client.connected()) {
      if(client.available()) {
        char requestChar = client.read();
        Serial.write(requestChar);

        if(requestChar == '\n') {

          if(currentHTTPRequestLine.length() == 0) {
            // Send HTTP Response using Class Interface
            
            HTMLDocument documentResponse(client);
            documentResponse.HTMLDocumentResponse(tempReading, humdReading, beatAvg, oxygenLvl);
            break;
          }
          else {
            if(firstRequestLine)
              recipientEmail = getPreferredEmail(currentHTTPRequestLine);
            firstRequestLine = false;
            Serial.println(recipientEmail);
            if(recipientEmail != "")
              emailSender.SetRecipientEmail(recipientEmail);
            Serial.println("This is recipientEmail" + recipientEmail);
            currentHTTPRequestLine = "";
          }
        }
        else if(requestChar != '\r') {
          currentHTTPRequestLine += requestChar;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }
}

void SendDataToSheet() {
  delay(1500);
  GoogleSheetsSender googleSheetsSender(7200, 0, "pool.ntp.org");
  googleSheetsSender.SendToGoogleSheet(tempReading, humdReading, beatAvg, oxygenLvl);
}

void lightLed(void * parameter) {
  while(true) {
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
    delay(100); 
  }
}

void heartRate() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
}

void oxygenLevel() {
  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  oxygenLvl = particleSensor.getSPO2(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange, millis());
}

void updateReadings() {
  tempReading = dht.readTemperature();
  humdReading = dht.readHumidity();
  heartRate();
  oxygenLevel();
}

void emailIndicators() {
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(18, HIGH);
}

void controlServo(void * parameter) {
  int buttonNew;
  int buttonOld = 1;
  int servoAngle = 52;
  while(true) {
    buttonNew = digitalRead(5);
    if(buttonOld == 0 && buttonNew == 1) {
      if(servoAngle != 128) {
        myservo.write(128);
        servoAngle = 128;
      }
      else {
        myservo.write(52);
        servoAngle = 52;
      }
    }
    buttonOld = buttonNew;
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(18, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);
  dht.begin();
  checkConnection();
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(2, 500, 2400); // attaches the servo on pin 18 to the servo object
  myservo.write(52);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  xTaskCreatePinnedToCore(controlServo, "controlservo", 3000, NULL, 1, &Task1, 0);
}

void loop() {
  clientConnection();
  if(millis() - sendToSheetDelay >= 5000) {
    SendDataToSheet();
    sendToSheetDelay = millis();
  }

  if(checker.checker(beatAvg, "heartrate") == "above" && checker.checker(oxygenLvl, "oxygenlevel") != "below") {
    sendEmail = true;
    if(millis() - sendEmailDelay >= 10000 && sendEmail == true) {
      emailIndicators();
      emailSender.SendEmail("Heart rate is high:" + String(beatAvg) + "bpm");
      sendEmail = false;
      sendEmailDelay = millis();
      digitalWrite(18, LOW);
    }
  }
  else if(checker.checker(beatAvg, "heartrate") != "above" && checker.checker(oxygenLvl, "oxygenlevel") == "below") {
    sendEmail = true;
    if(millis() - sendEmailDelay >= 10000 && sendEmail == true) {
      emailIndicators();
      emailSender.SendEmail("Oxygen level reduced: " + String(oxygenLvl) + "%");
      sendEmail = false;
      sendEmailDelay = millis();
      digitalWrite(18, LOW);
    }
  }
  else if(checker.checker(beatAvg, "heartrate") == "above" && checker.checker(oxygenLvl, "oxygenlevel") == "below") {
    sendEmail = true;
    if(millis() - sendEmailDelay >= 10000 && sendEmail == true) {
      emailIndicators();
      emailSender.SendEmail("Heart rate is high:" + String(beatAvg) + "bpm" + " and Oxygen level reduced: " + String(oxygenLvl) + "%");
      sendEmail = false;
      sendEmailDelay = millis();
      digitalWrite(18, LOW);
    }
  }
  updateReadings();
}
