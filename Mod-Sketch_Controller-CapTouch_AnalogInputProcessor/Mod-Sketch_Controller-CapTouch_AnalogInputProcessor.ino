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
#include "InputProcessor.h"

InputProcessor in0(30);
InputProcessor in0smooth(30);
InputProcessor in1(30);

void setup() {
  
  //QTOUCH setup
   // No pins to setup, pins can still be used regularly, although it will affect readings

   // Serial.begin(9600);
//These are reading the internal cap-touch analog inputs on the dilduino
    ref0 = ADCTouch.read(A2, 500);    //create reference values to 
    ref1 = ADCTouch.read(A3, 500);      //account for the capacitance of the pad
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
  modeCheck();
  serialProcessor();
 // captouchProcessor();
 in0.update(ADCTouch.read(A2,10));
controlval1 = in1.update(ADCTouch.read(A3,10));

Serial.print (in0.rawValue);
Serial.print(", scale ");
Serial.print (in0.scaledValue);
Serial.print(", buffavg ");
Serial.print (in0.getAverage());
Serial.print(", buffavgscaled ");
Serial.print (controlval0=in0.getscaledAverage());
Serial.print(", sum ");
Serial.print (in0.getBuffSum());
/*
Serial.print(", avgdiff ");
Serial.print (in0.getAvgDiff());
*/

Serial.print(", customthresh ");
Serial.print (in0.offset);
Serial.print(", threshavgdiff ");
Serial.print (constrain(in0.getoffsetAvgDiff()*5,0,255));

Serial.print(", calmin ");
Serial.print (in0.min);
Serial.print(", calMax ");
Serial.println (in0.max);
Toy.setLED(0, controlval0);

Serial.print(", STDEV ");
Serial.print (in0.getSTDEV());

}

void modeCheck() {
  if (in0.calmode) {
    in0.calibrate(3000, getBuffAvg0);
  }
//if(in1.calmode){
//  in1.calibrate(3000,in1.getAverage());
//}
}

int getCaptouch0(){
return ADCTouch.read(A2,100);

}


int getCaptouch1(){
return ADCTouch.read(A3,100);

}

int getBuffAvg0(){
 in0.update(ADCTouch.read(A2,10));
 return in0.getAverage();


}

void captouchProcessor(){
  
  
   qvalue0 = ADCTouch.read(A2,10);   //no second parameter ==  --> 100 samples
     qvalue1 = ADCTouch.read(A3,10);     //  

    qvalue0 -= ref0;       //remove offset
    qvalue1 -= ref1;


    Serial.print("CapTouch in port A2 and A3\t");
    Serial.print(qvalue0);             //return value

     Serial.print("\t");
    Serial.print(ref0);
    Serial.print("\t");
    Serial.println(qvalue1);
    
    controlval0=constrain((qvalue0+4)*8,0,255);//bumped up power for internal
    
            controlval1=constrain((qvalue1+4)*8,0,255); // bumped up power for internal

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
Toy.addPattern(all);
Toy.addPattern(third);
  
      Toy.addPattern(motormix);
    Toy.addPattern(cicada);
    Toy.addPattern(rainforest);
  Toy.addPattern(perlinSwarm);
  
  Toy.addPattern(sharpRamp);
  

  
  Toy.addPattern(movemotor);
  
   Toy.addPattern(shakeFlow);

 Toy.addPattern(shakeWave);


Toy.addPattern(thumper);
Toy.addPattern(fadeCos);

    Toy.addPattern(on_off);
Toy.addPattern(shiftingWaves);

  Toy.addPattern(dougaller);


  Toy.addPattern(perlinSwarmTime);

  

  Toy.addPattern(fadeSaw);
  Toy.addPattern(randomBlip);
  Toy.addPattern(singleRandomBlip);
  Toy.addPattern(singleRandomBlipinv);

  

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
  Toy.attachDoubleClick(calibrateAllInputs);
  Toy.attachLongPressStart(setoffset);
}


// Click handler. Currently moves to next pattern.
void click() {
  Toy.cyclePattern();
}


//Cannot make a handler that uses the timer
void calibrateAllInputs() {
//calibrate for 3 secs
in0.calmode=true;
in1.calmode=true;

}

void setoffset(){
in0.offset = in0.getAverage();
in1.offset = in1.getAverage();
}

// Double click handler Currently increases power.
void doubleClick() {
  Toy.increasePower();
}

// Click and hold handler. Currently decreases power.
void longPress() {
 
}


void halt() {
  Toy.stop();
}

////////////////////////////
//////////////////////////////
//////////////////////////////////





