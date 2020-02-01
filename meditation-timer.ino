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
  Serial.println("Start: rotary encoder.");
  

//int i = digitalRead(sw_pin);
//
//if(!i){
//    display_num(5);
//}
//  display_num(20);
}

void loop()
{
  static int pos = 0;
  encoder.tick();
  
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    Serial.print(newPos);
    Serial.println();
    pos = newPos;
  } // if

  sevseg.setNumber(newPos, 2);
  
  sevseg.refreshDisplay(); // Must run repeatedly
} // loop ()
// register shift? multiplexer?
// code the rotary encoder
// make the numbers blink before choosing
// start timer


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
