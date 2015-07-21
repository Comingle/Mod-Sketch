
/* Include the library */
#include <OSSex.h>

#include <Wire.h>
#include "WiiChuck.h"
WiiChuck nunchuck(c_update, z_update);




bool c_update() {
  return nunchuck.cPressed();
}
bool z_update() {
  return nunchuck.zPressed();
}

//Record Playback Items
//#define CALIB_MAX 512
//#define CALIB_MIN 100
#define SAMPLE_DELAY 50 // in ms// qtouch = 50ms always  at 100samples with one pade 100ms with two pads
#define MAX_SAMPLES 512
char recording[MAX_SAMPLES] = {255};
//uint8_t recButtonPin = 4;
uint8_t playBackPin = 11;//middle motor, all motors are 5,10,11

uint8_t ledPin = 13;

bool modeRec = false;

bool modePlay = false;
bool modePlayLoop = false;


#include "patterns.h"


void setup() {
  // Set ID. ALPHA (0) or BETA (1) are current options.
  // The sketch won't compile until you set this!
  Toy.setID(MOD);

  //Setup hackerport to read from I2C which is where nunchuck is
  Toy.setHackerPort(HACKER_PORT_I2C);



  //startup the nucnchucks
  nunchuck.begin();
  nunchuck.update();
  // Start the Serial console
  Serial.begin(9600);
  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();



  ///////////
  //RecPlay Items
//  pinMode(recButtonPin, INPUT);
  //  digitalWrite(recordButtonPin, HIGH);
  //  pinMode(playButtonPin, INPUT);
  //  digitalWrite(playButtonPin, HIGH);
  pinMode(ledPin, OUTPUT);

  //add all our clicks and patterns
  attachClicks();
  addPatterns();

}


void loop() {

  nunchuck.update();
  serialProcessor();
 // nunchuckprintstats();
  modeCheck();

}

//A large switch statement that helps us determine modes
//that need to be run outside of OSSEX patterns
void modeCheck() {


  if (modeRec) {
    recordPattern(playBackPin);
  }
  if (modePlayLoop) {
    modePlay=true;
  }
  if (modePlay) {
    playPattern(playBackPin);

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

    //Try charging up the blaster!
    // charge();

  } else  {
    Serial.print("-");
    //   discharge();

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














// note: the _in array should have increasing values
int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size - 1]) return _out[size - 1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while (val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos - 1]) * (_out[pos] - _out[pos - 1]) / (_in[pos] - _in[pos - 1]) + _out[pos - 1];
}



void addPatterns() {

  
   Toy.addPattern(cicada);
  Toy.addPattern(sharpRamp);
  
  Toy.addPattern(rainforest);
  Toy.addPattern(perlinSwarm);
  Toy.addPattern(perlinSwarmTime);


  Toy.addPattern(shakeFlow);

  Toy.addPattern(shakeWave);

  Toy.addPattern(shaker);
  Toy.addPattern(shakerAxes);

  Toy.addPattern(on_off);


  Toy.addPattern(dougaller);

  Toy.addPattern(movemotor);

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


//Nunchuck Funcs
  //Future function additions
    nunchuck.attachCClick(cycle);
        nunchuck.attachCDoubleClick(revcycle);

      nunchuck.attachCLongPressStart(halt); // emergency, SHUT IT DOWN, I JUST CAME

  //Z button is saved as a modifier to the patterns
  //nun.attachZClick(blaster);

  //But for right now, let's hook up Z to the Rec and Play

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
   Toy.reverseCyclePattern();
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

void playPattern(uint8_t playback) {
  uint16_t addr = 0;
  Serial.println("Playing");

  while (true) {

    //Uncomment if want to store to EEPROM
    //    uint8_t x = EEPROM.read(addr); //read it from memory
    //    Serial.print("Read EE: "); Serial.print(x);


    uint8_t x = recording[addr];

    Serial.print("Play recording: "); Serial.print(x);
    //Look for end of pattern flag!
    if (x == 255) break;



    Serial.print(" @ "); Serial.println(addr);
    analogWrite(playBackPin, x);
    delay(SAMPLE_DELAY);
    addr++;
    if (addr == 512) break;
  }
  Serial.println("Done Playback");
  //turn it back off
  analogWrite(playBackPin, 0);

  modePlay = false;
  //delay(250);
}


void playPatternLoop(uint8_t playback) {
  uint16_t addr = 0;
  Serial.println("Playing");

  while (true) {

    //Uncomment if want to store to EEPROM
    //    uint8_t x = EEPROM.read(addr); //read it from memory
    //    Serial.print("Read EE: "); Serial.print(x);


    uint8_t x = recording[addr];

    Serial.print("Play recording: "); Serial.print(x);
    //Look for end of pattern flag!
    if (x == 255) break;



    Serial.print(" @ "); Serial.println(addr);
    analogWrite(playBackPin, x);
    delay(SAMPLE_DELAY);
    addr++;
    if (addr == 512) break;
  }
  Serial.println("Done Playback");
  //turn it back off
  analogWrite(playBackPin, 0);

  modePlay = true;
  //delay(250);
}


void recordPattern(uint8_t playBack) {
    modePlay = false;

  uint16_t addr = 0;

  Serial.println("Recording");
  digitalWrite(ledPin, HIGH);

  while (z_update()) {



    //int     a = analogRead(A2);
  nunchuck.update();

    int     a = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 254); // limit to 254 for our signals

    Serial.print("Read analog: "); Serial.print(a);


    /* if (a < CALIB_MIN) a = CALIB_MIN;
     if (a > CALIB_MAX) a = CALIB_MAX;
     a = map(a, CALIB_MIN, CALIB_MAX, 0, 254);
     */

    //for analogread
    //      a = map(a, 0, 1024, 0, 254);

    Serial.print(" -> "); Serial.print(a);
    Serial.print(" @ "); Serial.println(addr);

    analogWrite(playBackPin, a);


    recording[addr] = a;
    //uncomment if want to store on eeprom
    //EEPROM.write(addr, a);


    addr++;
    if (addr == 512) break;
    //turn back on for regular analog read
    delay(SAMPLE_DELAY); // Qtouch already has a 100ms delay inherently
  }
  //turn off playback
  analogWrite(playBackPin, 0);

  //If we exit the loop early, then set as 255 to flag end of pattern recorded
  if (addr != 512) {
    //EEPROM.write(addr, 255);
    recording[addr] = 255;
  }

  //turn off pin
  digitalWrite(ledPin, LOW);

  Serial.println("Done rec");
  delay(250);
  modeRec = false;

}

