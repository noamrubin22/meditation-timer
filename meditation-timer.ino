#include <RotaryEncoder.h>
#include "SevSeg.h"
#include <Servo.h>
//#include <Serial.h>
//#include <SimpleTimer.h>

// initalizing the rotary encoder switch 
int sw_pin = A5;

// initalizing motor
Servo myservo; 

// define rotary input pins
RotaryEncoder encoder(A4, A3);

// initiate a seven segment controller object
SevSeg sevseg; 
//// initializing the timer object 
////SimpleTimer timer;

void setup() {
  // setting up 7segments display 
  byte numDigits = 4;
  byte digitPins[] = {A2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(90);
  
  // use pull-up as a internal resistor. 
  // if you want to read a button you should always use a pullup 
  // we are using this to prevent "floating"
  // https://learn.sparkfun.com/tutorials/pull-up-resistors/all
  // when the button is pushed, the current is LOW, otherwise HIGH
  pinMode(sw_pin, INPUT_PULLUP);

  // define motor pin
  myservo.attach(2);
  myservo.write(0);
  
  Serial.begin(57600);
}
// the different states we have
enum State {
  INIT,
  SELECT_TIME
};

// set variables
int countdown = 0;
unsigned long time_stamp = 0;
bool do_gong = false;
bool positioning_time = false;
static State state = SELECT_TIME;

void loop()
{
  static bool pressed = false;
  
  // Change the state between INIT and SELECT_TIME, each button press
  // if button is pressed set to true
  if(!digitalRead(sw_pin)) {
    pressed = true;
  }
  // when button is not pressed/released, set to false
  else if (digitalRead(sw_pin) && pressed) {
    pressed = false;
    // go to the next state
    state = (State)((state + 1) % 2);
  }

 // if we are in the configuration state, set countdown with chosen number
  if (state == SELECT_TIME){
    encoder.tick();
    countdown = encoder.getPosition();
    sevseg.setNumber(countdown, 2);
    sevseg.refreshDisplay();
    // start gong when time is set and add positioning time
    do_gong = true;
    positioning_time = true;
  } else {
  // if we are in the INIT state
      if (positioning_time){
        delay(5000);
        positioning_time = false;
      }
      // start countdown
      if (do_gong) {
       // do the gong, reset the counter and set countingdown flag to counting.
       myservo.write(135);
       delay(200);
       myservo.write(0);
       do_gong = false;
       time_stamp = millis();
      } else {
        display_countdown(countdown, time_stamp);
      }
 }
}

void display_countdown(int num, unsigned long time_stamp){

  int displayed;
  int counter = (num*60);
  int seconds = counter - ((millis() - time_stamp) / 1000);

  // display minutes and seconds, without negatives
  if (seconds < 0) {
    seconds = 0;
    do_gong = true;
  }
  else if (seconds < 60) {
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

// to do for next time: 
// remove negatives in SELECTION_TIME mode. 
// add a delay without messing with the display for positioning_time