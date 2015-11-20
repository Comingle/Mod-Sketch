/* Mod Sketch Testing out input processor */
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

//These will be in OSsex in the future
//Remove then
#define H0 24
#define H1 27

#include "patterns.h"
#include "InputProcessor.h"

InputProcessor in0(10);
InputProcessor in1(10);
void setup() {

  // Set ID. ALPHA (0) or BETA (1) are current options.
  // The sketch won't compile until you set this!
  Toy.setID(MOD);
  Toy.setHackerPort(HACKER_PORT_AIN);


//Setup the input
in0.setupInput((int)Toy.device.inPins[0]);
in1.setupInput((int)Toy.device.inPins[1]);

in0.calibrate(4000);
in1.calibrate(4000);

  //Alternatively setup AND calibrate
  //in0.setupInputandCal((int)Toy.device.inPins[0], 5000);
 
pinMode(led,OUTPUT);

 // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

//add all our clicks and patterns
  attachClicks();
  addPatterns();


  // Start the Serial console
  Serial.begin(9600);

}



void loop() {
in0.update();
in1.update();


  
  serialProcessor();
  inputProcessor();

}


//Trying to make this generic and Simple,
// Thus going to have this function calculate basic CHANGES in the stateof the sensors 
// ie not really care about positive or negative, just absolute change
void inputProcessor(){
   qvalue0 = analogRead(A7);   //gives a value between 0 - 1023
     qvalue1 = analogRead(A9);     //  not currently giving us anything
//    qvalue0 =  Toy.getInput(0);
//qvalue1 = Toy.getInput(1);

//Debuggings
// controlval0=constrain(map(qvalue0,0,1023,0,255),0,255); 
// controlval1=constrain(map(qvalue1,0,1023,0,255),0,255);

  controlval0=analogRead(A7);
 controlval1=analogRead(A9);

    qvalue0 -= ref0;      //Check values against baseline
    qvalue1 -= ref1;

// take absolute change
qvalue0 = abs(qvalue0);
qvalue1 = abs(qvalue1);


// The motor outputs have a max resolution of 255
//It makes things a bit easier to not have to always be downsampling from 1023-255
// controlval0=constrain(map(qvalue0,0,1023,0,255),0,255); 
// controlval1=constrain(map(qvalue1,0,1023,0,255),0,255);
    analogWrite(led,controlval0); 


   Serial.print("Hacker Port Inputs in port H0 (9) and H1 (6) \t");
    Serial.print(controlval0);             //return value
    Serial.print("\t");
    Serial.println(controlval1);
    
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
   
    Toy.addPattern(sharpRamp); 

  Toy.addPattern(cicada);

  Toy.addPattern(fadeCos);
Toy.addPattern(shakeWave);
  Toy.addPattern(movemotor);
  Toy.addPattern(rainforeststorm);

Toy.addPattern(thumper);

  Toy.addPattern(shiftingWaves);




      Toy.addPattern(alternate);

    Toy.addPattern(on_off);
      Toy.addPattern(dougaller);

    Toy.addPattern(first);
    Toy.addPattern(rainforest);
  Toy.addPattern(perlinSwarm);
 


  Toy.addPattern(fadeSaw); // +
  Toy.addPattern(randomBlip);
  Toy.addPattern(singleRandomBlip); // ++
  Toy.addPattern(singleRandomBlipinv);

  Toy.addPattern(sharpRamp);

  Toy.addPattern(pulse); // +++
  Toy.addPattern(pulseinv); // +++

  Toy.addPattern(flicker); // +++


//  Toy.addPattern(mostlyHarmless);

 // Toy.addPattern(fadeOffset);
  
  
//    Toy.addPattern(first);
//  Toy.addPattern(second);
//  Toy.addPattern(third);
//  Toy.addPattern(perlinSwarmTime);


 //  Toy.addPattern(shakeFlow);

}

void  attachClicks() {
  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(calibrateAllInputs);
  Toy.attachLongPressStart(longPress);
}


// Click handler. Currently moves to next pattern.
void click() {
  Toy.cyclePattern();
}

// Double click handler 
void calibrateAllInputs() {
//calibrate for 3 secs
in0.calibrate(3000);
in1.calibrate(3000);

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





