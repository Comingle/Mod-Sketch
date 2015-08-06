/* Mod Sketch v0.2 -- written by Craig Durkin / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys */

/* Include the library */
#include <OSSex.h>

#include <ADCTouch.h>

#include <Wire.h> // fickle library you have to include


//Cap touch sensing Setup
int led = 13;
int ref0, ref1;       //reference values to remove offset
int qvalue0;
int qvalue1;

int controlval0=0;
int controlval1=0;

#include "patterns.h"


void setup() {
  
  //QTOUCH setup
   // No pins to setup, pins can still be used regularly, although it will affect readings
Toy.setHackerPort(HACKER_PORT_AIN);
   // Serial.begin(9600);
//These are reading the internal cap-touch analog inputs on the dilduino
    ref0 = ADCTouch.read(A7, 500);    //create reference values to 
    ref1 = ADCTouch.read(A9, 500);      //account for the capacitance of the pad
pinMode(led,OUTPUT);

////////////
  
  // Set ID. ALPHA (0) or BETA (1) are current options.
  // The sketch won't compile until you set this!
  Toy.setID(MOD);
  
 // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

//add all our clicks and patterns
  attachClicks();
  addPatterns();


  // Start the Serial console
  Serial.begin(9600);

}



void loop() {
  
  serialProcessor();
  captouchProcessor();

}

void captouchProcessor(){
  
  
   qvalue0 = ADCTouch.read(A7,500);   //no second parameter
     qvalue1 = ADCTouch.read(A9,500);     //   --> 100 samples

    qvalue0 -= ref0;       //remove offset
    qvalue1 -= ref1;
   
    qvalue0 = abs(qvalue0); // in case we just want absolute difference
    qvalue1 = abs(qvalue1);

    Serial.print("CapTouch in port A2 and A3\t");
    Serial.print(qvalue0);             //return value
    Serial.print("\t");
    Serial.println(qvalue1);

    //The Qtouch from the hacker port is not immediately as sensitive
    controlval0=constrain(qvalue0*10,0,255);
    controlval1=constrain(qvalue1*10,0,255);

    analogWrite(led,controlval0); 
 //    delay(1); //delay is for the cap touch sensing
  
}
 
 
 
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




void addPatterns() {
  
  
    Toy.addPattern(first);
    Toy.addPattern(rainforest);
  Toy.addPattern(perlinSwarm);
  Toy.addPattern(perlinSwarmTime);

  
   Toy.addPattern(shakeFlow);

 Toy.addPattern(shakeWave);




    Toy.addPattern(on_off);


  Toy.addPattern(dougaller);

  Toy.addPattern(movemotor);

  //Toy.addPattern(thumper);


//  Toy.addPattern(rainforest);
 // Toy.addPattern(rainforeststorm);

  Toy.addPattern(shiftingWaves);

  Toy.addPattern(mostlyHarmless);

  Toy.addPattern(sharpRamp);
  Toy.addPattern(dougaller);

  Toy.addPattern(cicada);

  Toy.addPattern(fadeCos);

  Toy.addPattern(fadeSaw);
  Toy.addPattern(randomBlip);
  Toy.addPattern(singleRandomBlip);
  Toy.addPattern(singleRandomBlipinv);

  Toy.addPattern(sharpRamp);

  Toy.addPattern(pulse);
  Toy.addPattern(pulseinv);

  Toy.addPattern(flicker);



 // Toy.addPattern(fadeOffset);
  
  
//    Toy.addPattern(first);
//  Toy.addPattern(second);
//  Toy.addPattern(third);
}

void  attachClicks() {
  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(doubleClick);
  Toy.attachLongPressStart(longPress);
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


void halt() {
  Toy.stop();
}

////////////////////////////
//////////////////////////////
//////////////////////////////////





