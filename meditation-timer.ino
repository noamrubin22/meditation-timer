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
  
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false; // Default 'false' is Recommended, if true the display will blank while delaying
  bool leadingZeros = true;
  bool disableDecPoint = false;
  
  // sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
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
    // set the counter to zero if it going into minus values
    if (countdown < 0){
      countdown = 0;
      encoder.setPosition(countdown);
    }
    sevseg.setNumber(countdown, 2);
    sevseg.refreshDisplay();
    // start gong when time is set and add positioning time
    do_gong = true;
    positioning_time = true;
  } else {
  // if we are in the INIT state
      if (positioning_time){
        time_stamp = millis();

        while(millis() - time_stamp < 1000){
          char str[] = "----";
          sevseg.setChars(str);
          sevseg.refreshDisplay();
        }
        
        while(millis() - time_stamp < 2000){
          char str[] = "---";
          sevseg.setChars(str);
          sevseg.refreshDisplay();
        }

        while(millis() - time_stamp < 3000){
          char str[] = "--";
          sevseg.setChars(str);
          sevseg.refreshDisplay();
        }

        while(millis() - time_stamp < 4000){
          char str[] = "-";
          sevseg.setChars(str);
          sevseg.refreshDisplay();
        }

        while(millis() - time_stamp < 5000){
          char str[] = "";
          sevseg.setChars(str);
          sevseg.refreshDisplay();
        }

        
        positioning_time = false;
      }
      // start countdown
      if (do_gong) {
       // do the gong, reset the counter and set countingdown flag to counting.
       myservo.write(135);
       
       //delay(200);
       time_stamp = millis();
       while(millis() - time_stamp < 200){
        char str[] = "----";
        sevseg.setChars(str);
        sevseg.refreshDisplay();
       }

       
       myservo.write(0);

       time_stamp = millis();
       while(millis() - time_stamp < 200){
        char str[] = "----";
        sevseg.setChars(str);
        sevseg.refreshDisplay();
       }
       
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

// TODO: remove negatives in SELECTION_TIME mode. -----DONE-----
// TODO: add a delay without messing with the display for positioning_time
// TODO: ((( CLEAN UP AND REFACTORY )))
// TODO: add pause state by button press
// TODO: save last setup, see EEPROM https://www.arduino.cc/en/Reference/EEPROM
// TODO: instead of timers/time counters use preocessor timer interrupts and implement interrpt service routine
// TODO: use more states or implement state machine
// TODO: use adjustable value for waiting time while the servo is moving.
//       Calculate the duty time needed for 1 degree by reading the current position and mesuring the current position,
//       see https://www.arduino.cc/en/Reference/ServoRead
