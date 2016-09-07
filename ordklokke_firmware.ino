#include <Wire.h>
#include "RTClib.h" // https://github.com/adafruit/RTClib
#include "APA102.h" // https://github.com/pololu/apa102-arduino


RTC_DS1307 rtc;
APA102<A0, A1> ledStrip;

typedef struct {
  uint8_t from;
  uint8_t to;
} fromto;

fromto klokken = {0,6};
fromto er = {7,8};
fromto fem_til = {9,11};
fromto ti_til = {12,13};
fromto kvart_til = {14,18};
fromto over = {19,22};
fromto paa = {23,24};
fromto halv = {25,28};
fromto ett = {29,31};
fromto to = {32,33};
fromto tre = {34,36};
fromto fire = {37,40};
fromto fem = {41,43};
fromto seks = {44,47};
fromto syv = {48,50};
fromto aatte = {51,54};
fromto ni = {55,56};
fromto ti = {57,58};
fromto elleve = {59,64};
fromto tolv = {65,68};
fromto period = {69,69};

const uint16_t ledCount = 70;
const uint8_t brightness = 1;
rgb_color colors[ledCount];
rgb_color currentRGB;

void setup()
{
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // 
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  } else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  ledTEST();
}

void ledTEST() {
  for (int i=0; i<200; i++) {
    uint8_t time = millis() >> 2;
    for(uint16_t i = 0; i < ledCount; i++)
    {
      uint8_t x = time - i * 8;
      colors[i].red = x;
      colors[i].green = 255 - x;
      colors[i].blue = x;
    }
  
    ledStrip.write(colors, ledCount, brightness);
  
    delay(10);
  }
}

void allOff() {
  for(uint16_t i = 0; i < ledCount; i++)
  {
    colors[i].red = colors[i].green = colors[i].blue = 0;
  }
  ledStrip.write(colors, ledCount, 0);
}

void light(fromto &segment, rgb_color &color) {
  if (segment.from < ledCount && segment.to < ledCount) {
    for(uint16_t i = segment.from; i < segment.to; i++)
    {
      colors[i].red = color.red;
      colors[i].green = color.green;
      colors[i].blue = color.blue;
    }
  }
}

void loop()
{
  allOff();
  currentRGB.red = 255;
  currentRGB.green = 128;
  currentRGB.blue = 64;
  light(klokken, currentRGB);
  light(er, currentRGB);
  DateTime now = rtc.now();
  uint8_t hour = now.hour();
  uint8_t minute = now.minute();
  uint8_t second = now.second();
  uint8_t minzone = (minute+3) / 5; // we use five min increments

  colors[second].blue = 255;
  
  ledStrip.write(colors, ledCount, brightness);
}
