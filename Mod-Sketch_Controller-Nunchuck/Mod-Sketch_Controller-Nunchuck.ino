/* Mod Sketch with Nunchuck v0.5 -- written by Craig Durkin, Quitmeyer / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys
 *this is the super basic version of the main mod sketch that can toggle through
 * different patterns with no controllers attached or anything
 */

/* Include the library */
#include <OSSex.h>
#include <Wire.h>

//Include Wii Nunchuck Accessories
#include "WiiChuck.h"
WiiChuck nunchuck(c_update, z_update);

//This library makes the C and Z buttons work even with unofficial Wiichucks (e.g. 3rd party ones)
bool c_update() {
  return nunchuck.cPressed();
}
bool z_update() {
  return nunchuck.zPressed();
}


//Items for doing Recording and Playback
#define SAMPLE_DELAY 50 // in ms// qtouch = 50ms always  at 100samples with one pade 100ms with two pads
#define MAX_SAMPLES 512
char recording[MAX_SAMPLES] = {255}; //This sets the initial recording as blank (it stops reading at a full 255)
bool modeRec = false;
bool modePlay = false;
bool modePlayLoop = false;

//include the oscillator functions that drive the patterns
#include "oscillators.h"
//Include all the patterns we can control created in the neighboring file
#include "patterns.h"


void setup() {
  Toy.setID(MOD);

  //Setup hackerport to read from I2C which is where nunchuck is
  Toy.setHackerPort(HACKER_PORT_I2C);

  //startup the nucnchucks
  nunchuck.begin();
  nunchuck.update();

  // Start the Serial console
  Serial.begin(9600);

  //add all our clicks and patterns
  attachClicks();
  addPatterns();


  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();
}


void loop() {

  //Keep refreshing the values from the nunchuck
  nunchuck.update();
  //Show updates of all the Nunchuck stuff
  // nunchuckprintstats();


  //Keep getting new serial controls
  serialProcessor();


  //Determine what Recording or playback mode we are in.
  modeCheck();

}

//A simple switch statement that helps us determine modes
//that need to be run outside of OSSEX patterns
void modeCheck() {
  if (modeRec) {
    recordPattern();
  }
  if (modePlayLoop) {
    modePlay = true;
  }
  if (modePlay) {
    playPattern();

  }
}


void nunchuckprintstats() {
  Serial.print(nunchuck.readJoyX());
  Serial.print(", ");
  Serial.print(nunchuck.readJoyY());
  Serial.print(", ");

  if (z_update()) {
    Serial.print("Z");

  } else  {
    Serial.print("-");

  }

  Serial.print(", ");

  if (c_update()) {
    Serial.print("C");

  } else  {
    Serial.print("-");

  }

  Serial.print("   Roll: ");

  Serial.print(nunchuck.readRoll());
  Serial.print(", ");
  Serial.print(" Pitch ");

  Serial.print(nunchuck.readPitch());
  Serial.print(", ");

  Serial.print("   Accel Data ");

  Serial.print((int)nunchuck.readAccelX());
  Serial.print(", ");
  Serial.print((int)nunchuck.readAccelY());
  Serial.print(", ");

  Serial.print((int)nunchuck.readAccelZ());
  Serial.print(", ");

  Serial.print("  ChargePower ");

  Serial.print(chargepow);
  Serial.print(", ");
  Serial.println();
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





//ADD Patterns
//This function loads up all the patterns to be used in the current compilation.
//NOTE: comment out patterns if the memory gets to low, these patterns eat up memory
void addPatterns() {
  Toy.addPattern(cicada);
  Toy.addPattern(sharpRamp);

  Toy.addPattern(rainforest);
  Toy.addPattern(perlinSwarm);
  // Toy.addPattern(perlinSwarmTime);


  Toy.addPattern(shakeFlow);

  Toy.addPattern(shakeWave);

  Toy.addPattern(shaker);
  Toy.addPattern(shakerAxes);

  Toy.addPattern(on_off);


  Toy.addPattern(dougaller);

  // Toy.addPattern(movemotor);

  //Toy.addPattern(thumper);


  //  Toy.addPattern(rainforest);
  // Toy.addPattern(rainforeststorm);

  Toy.addPattern(shiftingWaves);

  Toy.addPattern(mostlyHarmless);

  Toy.addPattern(dougaller);



  Toy.addPattern(fadeCos);

  // Toy.addPattern(fadeSaw);
  //  Toy.addPattern(randomBlip);
  //  Toy.addPattern(singleRandomBlip);
  //  Toy.addPattern(singleRandomBlipinv);
  //
  //  Toy.addPattern(sharpRamp);
  //


  //  Toy.addPattern(pulse);
  //  Toy.addPattern(pulseinv);

  //  Toy.addPattern(flicker);
  //  Toy.addPattern(weird2);
  //  Toy.addPattern(weird3);

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

  //Nunchuck Clicks
  nunchuck.attachCClick(cycle);
  nunchuck.attachCDoubleClick(revcycle);
  nunchuck.attachCLongPressStart(halt); // emergency, SHUT IT DOWN, I JUST CAME

  //Z button is saved as a modifier to the patterns or for Record and Play
  nunchuck.attachZClick(playback);
  //  nunchuck.attachZDoubleClick(playbackloop); not ready for prime time
  nunchuck.attachZLongPressStart(record);

}

//*****
//Click Handler Functions
//*****
// Click handler. Currently moves to next pattern.
void click() {
  Toy.cyclePattern();
}

void cycle() {
  Toy.cyclePattern();
}
void revcycle() {
  Toy.previousPattern();
}

void playback() {
  modePlay = true;
  modePlayLoop = false;
}

void playbackloop() {
  modePlayLoop = true;
}
void record() {
  modeRec = true;
  modePlayLoop = false;

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

////////////////
///Different Mode Functions
///

void playPattern() {
  uint16_t addr = 0;
  Serial.println("Playing");

  while (true) {
    //Uncomment if want to store to / read from EEPROM
    //    uint8_t x = EEPROM.read(addr); //read it from memory
    //    Serial.print("Read EE: "); Serial.print(x);

    //Set all motors to the value
    uint8_t x = recording[addr];
    Serial.print("Play recording: "); Serial.print(x);

    //Look for end of pattern flag! a value of 255 indicates end of pattern
    if (x == 255) break;

    Serial.print(" @ "); Serial.println(addr);
    Toy.setOutput(-1, x);
    //analogWrite(playBackPin, x);
    delay(SAMPLE_DELAY);
    addr++;
    if (addr == MAX_SAMPLES) break;
  }

  //turn motors back off
  Serial.println("Done Playback");
  Toy.setOutput(-1, 0);

  modePlay = false;
}


void playPatternLoop() {
  uint16_t addr = 0;
  Serial.println("Playing");

  while (true) {

    uint8_t x = recording[addr];

    Serial.print("Play recording: "); Serial.print(x);
    //Look for end of pattern flag!
    if (x == 255) break;

    //Set all motors to the value
    Serial.print(" @ "); Serial.println(addr);
    Toy.setOutput(-1, x);

    delay(SAMPLE_DELAY);
    addr++;
    if (addr == 512) break;
  }

  //turn motors back off
  Toy.setOutput(-1, 0);
  Serial.println("Done Playback");

  //Keep LOOPING
  modePlay = true;
}


void recordPattern() {
  modePlay = false;

  uint16_t addr = 0;

  //Notify user we are in recoring mode
  Serial.println("Recording");
  Toy.setLED(0, 255);

  while (z_update()) {

    //Update that nunchuck each time
    nunchuck.update();
    int     a = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 254); // limit to 254 for our signals

    Serial.print("Read analog: "); Serial.print(a);

    Serial.print(" -> "); Serial.print(a);
    Serial.print(" @ "); Serial.println(addr);

    //Playback while recording
    Toy.setOutput(-1, a);
    recording[addr] = a;

    //uncomment if want to store on eeprom
    //EEPROM.write(addr, a);

    addr++;
    if (addr == 512) break; //End of our recording array

    //This delay can stretch out our limited resolution to cover different lengths of time
    delay(SAMPLE_DELAY);
  }
  //turn off playback
  // analogWrite(playBackPin, 0);
  Toy.setOutput(-1, 0);

  //End of Pattern flag
  //If we exit the loop early, then set as 255 to flag end of pattern recorded
  if (addr != 512) {
    //EEPROM.write(addr, 255);
    recording[addr] = 255;
  }

  //turn off Led to indicate recording is done
  Toy.setLED(0, 0);
  Serial.println("Done rec");

  delay(250); //Optional pause before doing anything else after recording
  modeRec = false;
}

