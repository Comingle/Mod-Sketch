/* Tonga base software -- written by Craig Durkin / Comingle. First released Aug 17, 2014 */
/* This software comes pre-loaded on Tonga sex toys */

#include <Comingle.h>

bool clicked = false;

void setup() {
  // 0 for all Tongas currently.
  Device.setID(0);
  
  // Button will increase/decrease power by 20%
  Device.setScale(0.2);
  
  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

  // Set the patterns that the button will cycle through
  Device.addPattern(first);
  Device.addPattern(second);
  Device.addPattern(third);
  Device.addPattern(pulse);
  Device.addPattern(fadeCos);
  Device.addPattern(weird3);
  Device.addPattern(flicker);
  Device.addPattern(weird2);
  Device.addPattern(fadeSequence);
  Device.addPattern(fadeOffset);
  Device.addPattern(pulse2);

  // Set up the button click handlers
  Device.attachClick(click);
  Device.attachDoubleClick(doubleClick);
  Device.attachLongPressStart(longPress);
  
}

void loop() {
  int m = 0;
  int upperThreshold = 460;
  int lowerThreshold = 430;
  int lowerLimit = 230;
  int upperLimit = 530;
  int out = 0;
  if (!clicked) {
    int in = Device.getInput(2);
    if (in < lowerThreshold && in > lowerLimit) {
      m = map(in, lowerThreshold, lowerLimit, 0, 255);
    } else if (in > upperThreshold && in < upperLimit) {
      m = map(in, upperThreshold, upperLimit, 0, 255);
      out = 2;
    } else {
      out = -1;
      m = 0;
    }
    Device.setOutput(out, m);
    delay(50);
  }
}

int step[3];
int* blip(int seq) {
  step[0] = -1;
  
  seq %= 5;

  if (seq % 2) {
    step[1] = 0;
  } else {
    step[1] = 200;
  }

  step[2] = 1000;

  if (seq == 4) {
    return NULL;
  } else {
    return step;
  }
}




// Cycle through all the outputs, turn the LED on and leave it on to show that we're on
void startupSequence() {
  int outs = Device.device.outCount;
  int outInterval = 100 / outs;
  
  Device.setLED(0,128);
    
  for (int i = 0; i < outs; i++) {
    Device.setOutput(i, 100+outInterval*i);
    delay(100);
    Device.setOutput(i, 0);
    delay(50);
  }
  for (int i = 0; i < 3; i++) {  
    Device.setLED(0,0);
    delay(100);
    Device.setLED(0,128);
    delay(100);
  }
}

void click() {
  clicked = true;
  Device.cyclePattern();
}

void doubleClick() {
  Device.increasePower();
}

void longPress() {
  Device.decreasePower();
}

