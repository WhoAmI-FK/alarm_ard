#include "Arduino.h"
#include "uRTCLib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_BUT2 2
#define PIN_BUT3 3
#define PIN_BUT4 4

bool alarmModeActive = false;
long alarmHour = 0;
long alarmMinute = 0;
long alarmSecond = 0;
long totalTime = 0;

// uRTCLib rtc;
uRTCLib rtc(0x68);
LiquidCrystal_I2C lcd(0x27, 20,4);

void setup() {
delay (2000);
  Serial.begin(9600);
  Serial.println("Serial OK");

  #ifdef ARDUINO_ARCH_ESP8266
    URTCLIB_WIRE.begin(0, 2); // D3 and D4 on ESP8266
  #else
    URTCLIB_WIRE.begin();
  #endif

  pinMode(PIN_BUT2, INPUT);
  pinMode(PIN_BUT3, INPUT);
  pinMode(PIN_BUT4, INPUT);
  pinMode(9, OUTPUT); // buzzer
       // ss m  h
  rtc.set(0, 0, 0, 6, 5, 21, 23);
  lcd.init();
  lcd.init();
  lcd.backlight();
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}

void loop() {
  rtc.refresh();
  if(!alarmModeActive){
  if(rtc.hour() == 0 && rtc.minute() == 0 && rtc.second() == 0)
  {
    lcd.clear();
  }
  // display current time
  lcd.setCursor(0,0);
  if(rtc.hour()<10){
    lcd.print('0');
  }
  lcd.print(rtc.hour());
  lcd.print(':');
  if(rtc.minute() < 10){
    lcd.print('0');
  }
  lcd.print(rtc.minute());
  lcd.print(':');
  if(rtc.second() < 10)
  {
    lcd.print('0');
  }
  lcd.print(rtc.second());
  lcd.setCursor(0,1);
  lcd.print(rtc.year());
  lcd.print('/');
  lcd.print(rtc.month());
  lcd.print('/');
  lcd.print(rtc.day());
  if(rtc.hour() == alarmHour &&
     rtc.minute() == alarmMinute &&
     rtc.second() == alarmSecond)
  {
    buzz();
  }
  }else{
   
  }
  // button handlign

   if(checkButtonv2(PIN_BUT2))
   {
    alarmModeActive = !alarmModeActive;
    if(!alarmModeActive) lcd.clear();
   }
   if(alarmModeActive)
   {
    lcd.setCursor(0,0);
    lcd.print("Setting alarm:");
    lcd.setCursor(0,1);
    if(alarmHour < 10){
      lcd.print('0');
    }
    lcd.print(alarmHour);
    lcd.print('/');
    if(alarmMinute < 10){
      lcd.print('0');
    }
    lcd.print(alarmMinute);
    lcd.print('/');
    if(alarmSecond < 10){
      lcd.print('0');
    }
    lcd.print(alarmSecond);

    while(checkButtonv2(PIN_BUT4))
    {
      totalTime++;
      if(totalTime == 86400){
        totalTime = 0;
      }

    }
       while(checkButtonv2(PIN_BUT3))
    {
      if(totalTime == 0)
      {
        totalTime = 86400;
      }
      totalTime--;

    }
    calcTime(&totalTime, &alarmSecond, &alarmMinute, &alarmHour);
   }
  
}


void calcTime(long* totalTimeInSec, long* alarmSecond, long* alarmMin, long* alarmHour)
{
  *alarmMin = (*totalTimeInSec) / 60;
  *alarmHour = (*alarmMin) / 60;
  *alarmSecond =  (*totalTimeInSec)%60;
}

byte lastButtonState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;

bool checkButtonv2(int BUTTON) {
  if (millis() - lastTimeButtonStateChanged > debounceDuration) {
      byte buttonState = digitalRead(BUTTON);
      if (buttonState != lastButtonState) {
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == HIGH) {
        // do an action, for example print on Serial
         return true;
      }
    }
  }
   return false;
} 

void buzz(){
  while(!checkButtonv2(PIN_BUT3) && !checkButtonv2(PIN_BUT4) && !checkButtonv2(PIN_BUT2))
  {
    lcd.setCursor(10,1);
    lcd.print("BZZ!"); // replace by bell
    tone(9, 100);
    delay(50);
    noTone(9);
    delay(100);
  }
  lcd.clear();
}
