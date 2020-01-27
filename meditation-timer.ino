#include "SevSeg.h"
#include <SimpleTimer.h>

SevSeg sevseg; //Initiate a seven segment controller object
// initializing the timer object 
//SimpleTimer timer;

/* sets up digits and segments for display */
void setup() {
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(90);
}

/* displays a countdown starting with a given time */
void loop() {
  int displayed;

  int counter = (22*60);
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
