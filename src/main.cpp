#include <Arduino.h>
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include "lilygo-t5_v23.h"

RTC_DS3231 rtc;


GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

const int pumpPin = 12;
const int valve1Pin = 27;
const int valve2Pin = 26;
const int valve3Pin = 25;

unsigned long duration;

SPIClass  SDSPI(VSPI);

const char* ssid = "POCOF4";
const char* password = "g47=m249";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);

struct WateringSchedule {
    int hour;
    int minute;
    int duration; // in minutes
    int valve;
    bool done;
};

WateringSchedule schedule[] = {
    {8, 47, 1, 1, false},
    {8, 48, 1, 2, false} 
};

void updateDisplay() { // Added parameter here
    DateTime now = rtc.now();

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
    // display.update();
}


void setup() {
    pinMode(pumpPin, OUTPUT);
    pinMode(valve1Pin, OUTPUT);
    pinMode(valve2Pin, OUTPUT);
    pinMode(valve3Pin, OUTPUT);
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
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init(); // enable diagnostic output on Serial
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);

    display.update();
    Serial.println("Setup done");
}



void checkWateringSchedule(DateTime now) {
    bool valveOpened = false; // added valveOpened variable

    for (auto& entry : schedule) {
        if (!entry.done && 
            ((now.hour() > entry.hour) || 
            (now.hour() == entry.hour && now.minute() >= entry.minute)) && 
            (now.hour() < entry.hour || (now.hour() == entry.hour && now.minute() < entry.minute + entry.duration))) {

            Serial.println("Watering triggered");

            switch (entry.valve) { // added switch statement for valve control
                case 1:
                    digitalWrite(valve1Pin, HIGH);
                    valveOpened = true;
                    break;
                case 2:
                    digitalWrite(valve2Pin, HIGH);
                    valveOpened = true;
                    break;
                case 3:
                    digitalWrite(valve3Pin, HIGH);
                    valveOpened = true;
                    break;
            }

            if (valveOpened) {
                digitalWrite(pumpPin, HIGH);
                digitalWrite(valve1Pin, HIGH);
                Serial.println("Pump and valve started");
                startMillis = millis();
                duration = entry.duration * 60000; // Store duration
                state = PUMP_AND_VALVE_STARTED;
            }



            digitalWrite(pumpPin, LOW);
            digitalWrite(valve1Pin, LOW);
            digitalWrite(valve2Pin, LOW);
            digitalWrite(valve3Pin, LOW);

            Serial.println("Pump and valve stopped");

            entry.done = true;
        }
    }
}

enum State {
    IDLE,
    PUMP_AND_VALVE_STARTED,
    PUMP_AND_VALVE_RUNNING,
    PUMP_AND_VALVE_STOPPED
};

State state = IDLE;
unsigned long startMillis;

void loop() {
    DateTime now = rtc.now();
    checkWateringSchedule(now);

    switch (state) {
        case PUMP_AND_VALVE_STARTED:
          if (millis() - startMillis < duration) {
                state = PUMP_AND_VALVE_RUNNING;
            }
    }
    updateDisplay();
}