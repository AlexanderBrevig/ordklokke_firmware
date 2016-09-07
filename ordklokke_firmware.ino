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

const uint8_t SW1 = 8;
const uint8_t SW2 = 9;
const uint8_t SW3 = 10;

void setup()
{
  Serial.begin(57600);
  pinMode(SW1, INPUT); //TODO: add pulldown resistors
  pinMode(SW2, INPUT); 
  pinMode(SW3, INPUT); 
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
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

fromto &fromToFromHour(uint8_t hour) {
  return ett;
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

void readInputs() {
  TimeSpan plusMin(0,0,1,0);
  TimeSpan minusMin(0,0,-1,0);
  TimeSpan plusHr(0,1,0,0);
  TimeSpan minusHr(0,-1,0,0);
  uint8_t sw1 = digitalRead(SW1) == HIGH;
  uint8_t sw2 = digitalRead(SW2) == HIGH;
  uint8_t sw3 = digitalRead(SW3) == HIGH;
  if (!sw1 && !sw2 && !sw3) return;
  
  DateTime now = rtc.now();
  DateTime adj(now + (sw1 ? 
    (sw2?plusHr:minusHr) : (sw2?plusMin:minusMin)
  ));
  rtc.adjust(adj);
  delay(30); //stop from spamming so its easier to hit the mark by clicking
}
void loop()
{
  readInputs();
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
  uint8_t minzone = ((minute+3)%60) / 5; // we use five min increments

  if (minzone <=8 && minzone >=0){
    light(fromToFromHour(hour), currentRGB); //light current hour
  } else {
    light(fromToFromHour(hour+1), currentRGB); //light next hour
  }

  if ((minzone<=3 && minzone>=1) || (minzone<=8 && minzone>=7)) {
    light(over, currentRGB);
  } else if ((minzone<=5 && minzone>=4) || (minzone<=11 && minzone>=9)) {
    light(paa, currentRGB); //light next hour
  }
  
  if (minzone<=8 && minzone>=4) {
    light(halv, currentRGB);
  }
  
  if (minzone==1 || minzone==5 || minzone==7 || minzone==11) {
    light(fem_til, currentRGB);
  }
  if (minzone==2 || minzone==4 || minzone==8 || minzone==10) {
    light(ti_til, currentRGB);
  }
  if (minzone==3 || minzone==9) {
    light(kvart_til, currentRGB);
  }
      
  
  colors[second + 9].blue = 255; //start at letter on fem
  
  ledStrip.write(colors, ledCount, brightness);
}
