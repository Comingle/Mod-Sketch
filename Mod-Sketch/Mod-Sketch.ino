/* Mod Sketch v0.2 -- written by Craig Durkin / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys */

/* Include the library */
#include <OSSex.h>

bool clicked = false;

void setup() {
  // Set ID. ALPHA (0) or BETA (1) are current options.
  // The sketch won't compile until you set this!
  Toy.setID();
  
  // Button will increase/decrease power by 20%
  Toy.setPowerScale(0.2);
  
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
  Toy.addPattern(flicker);
  Toy.addPattern(weird2);
  // Toy.addPattern(fadeSequence);
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
  // Serial console. Read a character in to command[1], and a value in to val
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
  clicked = true;
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

// Begin Pattern functions

// Step variable for storing next pattern step. Must be global.
int step[3];

// Turn on all outputs slightly offset from each other. Continues to generate steps even after outputs are on so that the power can be adjusted.
int* flicker(int seq) {
  if (seq > 2) {
    step[2] = 200;
  } else {
    step[2] = 20;
  }
  seq %= 3;
  step[0] = seq;
  step[1] = 80;
 
  return step;
}


// Randomly blip an output on for a short burst.
int* pulse(int seq) {
  if (seq % 2) {
    step[0] = -1;
    step[1] = 0;
  } else {
    step[0] = random(0,3);
    step[1] = 144;
  }
  step[2] = 70;
  return step;
}

// Opposite of pulse() -- turn on all outputs, randomly blip one off
int* pulse2(int seq) {
  if (seq % 2) {
    step[0] = -1;
    step[1] = 100;
  } else {
    step[0] = random(0,3);
    step[1] = 0;
  }
  step[2] = 100;
  return step;
}

// fade output 0 in and out, fade output 1 in as 0 is fading out, fade output 2 in as 1 as fading out.
int* fadeOffset(int seq) {

  // sequence runs: 0 -> 255 -> 0 in 5 step increments.
  // 0 -> 255: 51 steps.
  // 255 -> 0: 51 steps;
  // -> 102 steps per output, 306 steps total.
  // normalize sequence
  seq %= 307;
  
  // output 0
  if (seq <= 51) {
    step[0] = 0;
    step[1] = fadeNormalize(seq);
    // output 0 and 1
  } else if (seq <= 154) {
    if (seq % 2) {
      step[0] = 1;
      step[1] = 2.5*(seq-51);
      step[2] = 30;
    } else {
      step[0] = 0;
      step[1] = 255 - 2.5*(seq-52);
      step[2] = 30;
    }
    // outputs 1 and 2
  } else if (seq <= 256) {
    if (seq % 2) {
      step[0] = 2;
      step[1] = 2.5*(seq-153);
      step[2] = 30;
    } else {
      step[0] = 1;
      step[1] = 255 - 2.5*(seq-154);
      step[2] = 30;
    }
    // output 2
  } else {
    step[0] = 2;
    step[1] = fadeNormalize(seq);
  }
  
  step[2] = 30;
  return step;
}

// First motor only
int* first(int seq) {
  step[0] = 0;
  step[1] = 100;
  step[2] = 50;
  return step;
}

// Second motor only
int* second(int seq) {
  step[0] = 1;
  step[1] = 100;
  step[2] = 50;
  return step;
}

// Third motor only
int* third(int seq) {
  step[0] = 2;
  step[1] = 100;
  step[2] = 50;
  return step;
}

int* weird2(int seq) {
  step[0] = 0;
  step[2] = 30;
  step[1] = round(127*cos(tan(tan(seq/(8*PI)))-PI/2)+127);
  return step;
}

int* weird3(int seq) {
  step[0] = 2;
  step[2] = 30;
  step[1] = round(50*(cos(seq/(8*PI)+PI/2) + sin(seq/2))+100);
  return step;
}

int *fadeCos(int seq) {
  step[0] = -1;
  step[2] = 50;
  step[1] = round(127 * cos((seq / (8*PI))-PI) + 127);
  return step;
}

// This pattern is described as boring! Use at your own risk.
int* fadeSequence(int seq) {
  step[2] = 30;

  // sequence runs: 0 -> 255 -> 0 in 5 step increments.
  // 0 -> 255: 51 steps.
  // 255 -> 0: 51 steps;
  // -> 102 steps per output, 306 steps total.
  // normalize sequence
  seq %= 307;
  
  if (seq <= 102) {
    step[0] = 0;
  } else if (seq <= 204) {
    step[0] = 1;
  } else {
    step[0] = 2;
  }
  
  step[1] = fadeNormalize(seq);
  return step;
}

// outputs the proper power level for our fade sequence. we know that it's 102 steps to go from 0 to 255 back to 0 in 5 step increments.
int fadeNormalize(int seq) {
  seq %= 102;
  if (seq <= 51) {
    return 5 * seq;
  } else {
    return 255 - 5*(seq-51);
  }
}




