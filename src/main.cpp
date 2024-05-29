/*
Board: LilyGo-T5 V2.3.1_2.13 Inch E-Paper
DriverChip:   GDEW0213T5
  - DEPG0213BN  : greylevel=2
  - GDEM0213B74 : greylevel=4
Product page: https://www.aliexpress.com/item/1005003063164032.html
Github: https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series/
Example:
  - Hello World: https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series/blob/master/examples/GxEPD_Hello_world/GxEPD_Hello_world.ino
*/

#include <Arduino.h>
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include "lilygo-t5_v23.h"
#include "FertigationOutput.h"

RTC_DS3231 rtc;

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

SPIClass  SDSPI(VSPI);

#define NUM_OUTPUS 3
#define MOTOR_PIN 12
#define LED_BLINK_PIN 0
const uint8_t arOutputsPin[NUM_OUTPUS] = {27, 26, 25};

//durasi output dalam milisecond
uint16_t arOutputOnDuration[NUM_OUTPUS] = {8000, 8000, 8000};

const char* ssid     = "POCOF4";
const char* password = "g47=m249";

// const char* weatherApiKey = "your_openweathermap_api_key";
// const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=your_city&units=metric&appid=";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // 1 hour offset, 60s update interval

FertigationOutput fertigationOutput;
Ticker ticker;
Ticker ledBlinkOff;

//   String getWeatherInfo() {
//   HTTPClient http;
//   http.begin(String(weatherApiUrl) + weatherApiKey);
//   int httpCode = http.GET();
//   if (httpCode > 0) {
//     String payload = http.getString();
//     DynamicJsonDocument doc(1024);
//     deserializeJson(doc, payload);
//     String weatherInfo = doc["weather"][0]["description"].as<String>();
//     float temperature = doc["main"]["temp"].as<float>();
//     return "Weather: " + weatherInfo + ", Temp: " + String(temperature) + "C";
//   } else {
//     return "Failed to get weather info";
//   }
//   http.end();
// }
void updateDisplay() {
  DateTime now = rtc.now();

  //buat ngecek apakah waktu sudah di set atau belum di local
  Serial.print("RTC time: ");
  Serial.println(now.timestamp(DateTime::TIMESTAMP_FULL));

  char dateTime[20];
  sprintf(dateTime, "%02d/%02d/%04d %02d:%02d:%02d", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0, 15);
  display.println("Yayasan Kebon Dalem");
  display.setCursor(0, 30);
  display.println(dateTime);

  if (digitalRead(12) == HIGH) {
    display.println("Pump: ON");
  } 
  else {
    display.println("Pump: OFF");
  }

  if (digitalRead(27) == HIGH) {
    display.println("Valve 1: ON");
  } else {
    display.println("Valve 1: OFF");
  }

  if (digitalRead(26) == HIGH) {
    display.println("Valve 2: ON");
  } else {
    display.println("Valve 2: OFF");
  }

  if (digitalRead(25) == HIGH) {
    display.println("Valve 3: ON");
  } else {
    display.println("Valve 3: OFF");
  }

  display.update();
}


void onTimer() {
  Serial.println("onTimer");
  digitalWrite(LED_BLINK_PIN, HIGH);
  
  ledBlinkOff.once_ms(100, []() { // turn off led after 100ms
    digitalWrite(LED_BLINK_PIN, LOW);
  });
  fertigationOutput.start();
  updateDisplay();
}

//String message;
void setup() {
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(LED_BLINK_PIN, OUTPUT);
    pinMode(arOutputsPin[27], OUTPUT);
    pinMode(arOutputsPin[26], OUTPUT);
    pinMode(arOutputsPin[25], OUTPUT);
    delay(100);
    Serial.begin(115200);

    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC"); 
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - startTime > 10000) { // 10 seconds timeout
          Serial.println("Failed to connect to WiFi within 10 seconds.");
          break;
      }
    delay(2000);
    Serial.println("Connecting to WiFi...");
    }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
     // Initialize NTP client
    timeClient.begin();
    timeClient.update();
    //buat ngecek apakah waktu sudah di set atau belum
    Serial.print("NTP time: ");
    Serial.println(timeClient.getFormattedTime());
  // Set RTC time
     rtc.adjust(DateTime(timeClient.getEpochTime()));
   }

    for (uint8_t i = 0; i < NUM_OUTPUS; i++) {
      fertigationOutput.addSolenoidOutput(arOutputsPin[i], arOutputOnDuration[i]);
    }
    // this will turn on motor pump after 200ms and turn off after 500ms
    fertigationOutput.setMotorPumpPin(MOTOR_PIN, 200, 500);
    ticker.attach(5, onTimer); // every 5s
    fertigationOutput.begin();

    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init(); // enable diagnostic output on Serial
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);

    display.update();
Serial.println("Setup done");
}



void loop() {
  // Get weather info and display it
  // String weatherInfo = getWeatherInfo();
  // display.setCursor(0, 60);
  // display.println(weatherInfo);
  // display.update();

    // Check if connected to WiFi before updating NTP client
//  if (WiFi.status() == WL_CONNECTED) {
//     // Update NTP client
//     timeClient.update();

//     //buat ngecek apakah waktu sudah di set atau belum
//     Serial.print("NTP time: ");
//     Serial.println(timeClient.getFormattedTime());
    
//      // Adjust RTC if necessary
//      rtc.adjust(DateTime(timeClient.getEpochTime()));
//}

  //updateDateTimeDisplay();
  fertigationOutput.update();
  //updateDisplay();
}