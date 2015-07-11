
/* Include the library */
#include <OSSex.h>

#include <Wire.h>
#include "WiiChuck.h"
WiiChuck nunchuck(c_update, z_update);

#include "patterns.h"



bool c_update() {
  return nunchuck.cPressed();
}
bool z_update() {
  return nunchuck.zPressed();
}



void setup() {
  // Set ID. ALPHA (0) or BETA (1) are current options.
  // The sketch won't compile until you set this!
  Toy.setID(BETA);
  Toy.setHackerPort(HACKER_PORT_I2C);

  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();


  // Start the Serial console
  Serial.begin(9600);

  nunchuck.begin();
  nunchuck.update();

  attachClicks();
  addPatterns();

}

void halt() {
  Toy.stop();
}

void loop() {


  serialProcessor();
  nunchuckprintstats();

}

void nunchuckprintstats() {
  nunchuck.update();
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
    Toy.addPattern(on_off);

  Toy.addPattern(first);
  Toy.addPattern(second);

  Toy.addPattern(third);
  Toy.addPattern(dougaller);

  Toy.addPattern(movemotor);

  Toy.addPattern(thumper);


  Toy.addPattern(rainforest);
  Toy.addPattern(rainforeststorm);

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
  Toy.addPattern(weird2);
  Toy.addPattern(weird3);

  Toy.addPattern(fadeOffset);
}

void  attachClicks() {

  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(doubleClick);
  Toy.attachLongPressStart(longPress);
  nunchuck.attachZClick( click);
  //  nunchuck.attachCClick( halt); //emergency, i just came! shut it down

  // nunchuck.attachCClick( blaster); //emergency, i just came shut down
  nunchuck.attachCClick( chargeblaster); //emergency, i just came shut down
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



