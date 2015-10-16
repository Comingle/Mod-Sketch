/* Mod Sketch v0.5 -- written by Craig Durkin, Quitmeyer / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys */
/*this is the super basic version of the main mod sketch that can toggle through
 * different patterns with no controllers attached or anything
 */

/* Include the library */
#include <OSSex.h>
//this is a meddle-some library that is in Arduino, but due to weird problems in Arduino, needs to be included
#include <Wire.h>

//include the oscillator functions that drive the patterns
#include "oscillators.h"
//Include all the patterns created in the neighboring file
#include "patterns.h"


void setup() {

  //In the future when there are different versions of the toys, you can select it with the
  //command below. for right now it just defaults to the MOD if not set
  // Toy.setID(MOD);

  // Start the Serial console
  Serial.begin(9600);

  //Attach functions to the different types of button clicks
  attachClicks();

  // Set the patterns that the button will cycle through. Toy will do nothing on startup,
  //and clicking the button once will run the 'first' pattern
  // Clicking again will run 'second', etc.
  addPatterns();

  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();
}


/* 
 *  Main Loop, 
 *  Not used as much for patterns
 *  but you can add extra functions you might want to be running
 *  continuously
 *  like the serialProcessor function
 */
void loop() {

  //This is the main function running in the loop. It allows you to control the dilduino
  //from a serial interface
  serialProcessor();

  //Most of the other commands (like the running of the patterns)
  // are taken care of more asynchronously outside the loop

}


/*
 * Serial Processor
 * Takes in data from the serial port to control 
 * the functions of the dilduino over a variety of interfaces
 */
void serialProcessor()
{
  // Serial console. Read a character in to command[1], and a value in to val
  char command[1];
  byte val;
  if (Serial.available() > 0) {
    Serial.readBytes(command, 1);
    if (command[0] == 'l') { // Set LED power
      val = Serial.parseInt();
      Toy.setLED(0, val);
      Serial.println(val);
    } else if (command[0] == '0' || command[0] == '1' || command[0] == '2') { // Set power of individual motor
      val = Serial.parseInt();
      Toy.setOutput(command[0], val);
      Serial.println(val);
    } else if (command[0] == '-') { // Catch '-1', set power of all motors
      int out = Serial.parseInt();
      out *= -1;
      val = Serial.parseInt();
      Toy.setOutput(out, val);
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

/*
 * Startup Sequence
 * This is the basic greeting
 * your Dildo gives you to show you that everything is working
 */
// Cycle through all the outputs, turn the LED on and leave it on to show that we're on
void startupSequence() {
  int outs = Toy.device.outCount;
  int outInterval = 100 / outs;

  Toy.setLED(0, 128);

  for (int i = 0; i < outs; i++) {
    Toy.setOutput(i, 100 + outInterval * i);
    delay(100);
    Toy.setOutput(i, 0);
    delay(50);
  }
  for (int i = 0; i < 3; i++) {
    Toy.setLED(0, 0);
    delay(100);
    Toy.setLED(0, 128);
    delay(100);
  }
}

/*
 * Click Handlers
 * Functions to map onto the different clicks of all
 * the different buttons available
 */

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



/*
 * Adding Patterns
 * All the patterns to be loaded into the toy's queue are loaded up here
 */

void addPatterns() {
  
  Toy.addPattern(first);
  Toy.addPattern(second);

  Toy.addPattern(third);
  Toy.addPattern(fadeCos);
  Toy.addPattern(cicada);

 // Toy.addPattern(rainforest);
  Toy.addPattern(rainforeststorm);
Toy.addPattern(thumper);
  Toy.addPattern(shiftingWaves);

  

  Toy.addPattern(sharpRamp);
  
Toy.addPattern(dougaller);
  

  

  Toy.addPattern(fadeSaw);
  Toy.addPattern(randomBlip);
  Toy.addPattern(singleRandomBlip);
  Toy.addPattern(singleRandomBlipinv);

  Toy.addPattern(sharpRamp);
Toy.addPattern(onetwothree);

  Toy.addPattern(mostlyHarmless);
  Toy.addPattern(on_off);

  Toy.addPattern(pulse);
  Toy.addPattern(pulseinv);

  Toy.addPattern(flicker);
  Toy.addPattern(weird2);
  Toy.addPattern(weird3);

  Toy.addPattern(fadeOffset);
}

void  attachClicks() {

  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(doubleClick);
  Toy.attachLongPressStart(longPress);
}
