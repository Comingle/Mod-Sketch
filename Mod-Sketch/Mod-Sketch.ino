/* Mod Sketch v0.2.1 -- written by Craig Durkin / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys */

// Include necessary libraries: Wire.h and WiiChuck.h for the nunchuck, OSSex.h for the Mod library
#include <Wire.h>
#include <OSSex.h>
#include <WiiChuck.h>

// Add the pattern library.
#include "patterns.h"

void setup() {
  // Button will increase/decrease power by 20%
  Toy.setPowerScaleStep(0.2);
  
  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

  // Set the patterns that the button will cycle through. Toy will do nothing on startup, and clicking the button once will run the 'first' pattern
  // Clicking again will run 'second', etc.

  Toy.addPattern(first);
  Toy.addPattern(second);
  Toy.addPattern(third);
  Toy.addPattern(pulse);
  Toy.addPattern(fadeCos);
  Toy.addPattern(weird3);
  Toy.addPattern(sharpRamp);
  Toy.addPattern(flicker);
  Toy.addPattern(weird2);
  Toy.addPattern(fadeOffset);
  Toy.addPattern(pulse2);

  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(doubleClick);
  Toy.attachLongPressStart(longPress);

  // Start the Serial console
  Serial.begin(9600);
}



void loop() {
  serialProcessor();
}

// Serial console. Read a character in to command[1], and a value in to val
void serialProcessor() { 
  char command[1];
  byte val;

  if (Serial.available() > 0) {
    Serial.readBytes(command,1);
    if (command[0] == 'l') { // Set LED power
      val = Serial.parseInt();
      Toy.setLED(0,val);
      Serial.println(val);
    } else if (command[0] == '0' || command[0] == '1' || command[0] == '2') { // Set power of individual motor
      val = Serial.parseInt();
      Toy.setOutput(command[0], val);
      Serial.println(val);
    } else if (command[0] == '-') { // Catch '-1', set power of all motors
      int out = Serial.parseInt();
      out *= -1;
      val = Serial.parseInt();
      Toy.setOutput(out,val);
    } else if (command[0] == '{') {
      int motors[3];
      motors[0] = Serial.parseInt();
      motors[1] = Serial.parseInt();
      motors[2] = Serial.parseInt();
      for (int i = 0; i < 3; i++) {
        if (motors[i] >= 0) {
          Toy.setOutput(i, motors[i]);
        }
      }
    } else if (command[0] == 'p') {
      Serial.println(Toy.decreasePower());
    } else if (command[0] == 'P') {
      Serial.println(Toy.increasePower());
    } else if (command[0] == 't') { // Decrease pattern time, as in everything goes faster
      Serial.println(Toy.decreaseTime());
    } else if (command[0] == 'T') {
      Serial.println(Toy.increaseTime());
    } else if (command[0] == 'r') { // Run a specific pattern, r,0; r,1; etc.
      val = Serial.parseInt();
      Toy.runPattern(val);
      Serial.println(Toy.getPattern());
    } else if (command[0] == 'g') {  // Get number of currently running pattern
      Serial.println(Toy.getPattern());
    } else if (command[0] == 's') {
      Toy.stop();
    } else if (command[0] == 'c') {
      Toy.cyclePattern();
      Serial.println(Toy.getPattern());
    } else if (command[0] == 'i') { // Read input 0 or 1 and print it to serial port
      int in = Serial.parseInt();
      in %= Toy.device.inCount;
      Serial.println(Toy.getInput(in));
    }
  }
}


// Cycle through all the outputs, turn the LED on and leave it on to show that we're on
void startupSequence() {
  int outs = Toy.device.outCount;
  int outInterval = 100 / outs;

  Toy.setLED(0,128);

  for (int i = 0; i < outs; i++) {
    Toy.setOutput(i, 100+outInterval*i);
    delay(100);
    Toy.setOutput(i, 0);
    delay(50);
  }
  for (int i = 0; i < 3; i++) {
    Toy.setLED(0,0);
    delay(100);
    Toy.setLED(0,128);
    delay(100);
  }
}

// Click handler. Currently moves to next pattern.
void click() {
  Toy.cyclePattern();
}

// Double click handler Currently increases power.
void doubleClick() {
 Toy.increasePower();
}

// Click and hold handler. Currently decreases power.
void longPress() {
  Toy.decreasePower();
}


