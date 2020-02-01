#include <RotaryEncoder.h>
#include "SevSeg.h"
//#include <Serial.h>
#include <SimpleTimer.h>

int sw_pin = A5;
int num;

RotaryEncoder encoder(A4, A3);

SevSeg sevseg; //Initiate a seven segment controller object
// initializing the timer object 
//SimpleTimer timer;

void setup() {
byte numDigits = 4;
byte digitPins[] = {2, 3, 4, 5};
byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
sevseg.setBrightness(90);

// use pull-up as a internal resistor. 
// if you want to read a button you should always use a pullup
pinMode(sw_pin, INPUT_PULLUP);

Serial.begin(57600);
}

bool flag = 0;

enum State {
  INIT,
  SELECT_TIME
};

int countdown = 0;
int newPos = 0;
int previousMillis=0;
int interval = 300;

void loop()
{
  static bool pressed = false;
  static State state = INIT;
  
  if(!digitalRead(sw_pin) )
    pressed = true;
  else if (digitalRead(sw_pin) && pressed) {
    pressed = false;
    state = (State)((state + 1) % 2);
  }
    
 if (state == SELECT_TIME){
  encoder.tick();
  newPos = encoder.getPosition();
  sevseg.setNumber(newPos, 2);
  sevseg.refreshDisplay();
  countdown = newPos;
  flag = true;
 } else {
  if(flag){
    display_num(countdown);
  } else {
    sevseg.setNumber(countdown, 2);
    sevseg.refreshDisplay(); // Must run repeatedly
  }
 }
    //display_num(newPos);
    //sevseg.blank();
}

void display_num(int num){

  int displayed;
  int counter = (num*60);
  int seconds = counter - (millis() / 1000);
  
  if (seconds < 60) {
    displayed = seconds;
  } else {
    int minutes;
    int dis_sec;
    minutes = seconds/60 *100;
    dis_sec = seconds % 60;
    displayed = minutes + dis_sec;   
  }
  
  sevseg.setNumber(displayed, 2); 
  sevseg.refreshDisplay(); // Must run repeatedly
}
