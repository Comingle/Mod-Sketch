/* Mod Sketch v0.2.5 -- written by Craig Durkin / Comingle. */
/* Additional Commands & Documentation added by Holger 'Lynxx' Hippenstiel */
/* This software comes pre-loaded on Comingle Mod sex toys */

// Include necessary libraries: Wire.h and WiiChuck.h for the nunchuck, OSSex.h for the Mod library
#include <Wire.h>
#include <OSSex.h>
#include <WiiChuck.h>

// Add the pattern library.
#include "patterns.h"

/*
Available commands over Serial:
- <n>         = Set Value for all Motors : Returns nothing
0 <n>         = Set Value for Motor 1 : Return current Value
1 <n>         = Set Value for Motor 2 : Return current Value
2 <n>         = Set Value for Motor 3 : Return current Value
c             = Cycle Pattern : Returns running pattern
g             = Get number of currently running pattern : Returns running pattern
i             = Read input 0 or 1 : Returns value of input
l <n>         = Set LED power : Return current Value
p             = Decrease Power : Returns new Powerlevel
P             = Increase Power : Returns new Powerlevel
r <n>         = Run a specific pattern, r,0; r,1; etc. : Returns running pattern
s             = Stop all Motors, reset Patternnumber, Power & Timescale : Returns nothing
t             = Decrease pattern time, as in everything goes faster : Returns new Timescale
T             = Increase pattern time, as in everything goes slower : Returns new Timescale
{<n>,<n>,<n>} = Set Value for Motor1,2 and 3 in a single line : Returns nothing

New from v0.2.2:
G = get Powerlevel : Returns Powerlevel
v = get Version : Returns Version information

New from v0.2.4:
U ... = Play the User transmitted pattern, for example:
U 3,255 0 0 500, 0 0 255 500, 0 255 0 500
3 Entries
First Motor 255, Second 0, Third 0, for 500ms
First Motor 0, Second 0, Third 255, for 500ms
First Motor 0, Second 255, Third 0, for 500ms

allStop Pattern added

New from v0.2.5:
u = Returns current Userpattern
*/

const String txtVersion = "Mod-Sketch v0.2.5";
int* userPattern; // Will dynamicly be allocated/freed
int userPatternSize = 0;
int userPatternNumber = 0; // Number of the Userpattern
int currentPatternNumber = 0; // Currently added Pattern number

void setup() {
  Toy.setID(); // Set Toy ID using default Value, currently 1 = MOD
  
  // Button will increase/decrease power by 20%
  Toy.setPowerScaleStep(0.2);
  
  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

  // Set the patterns that the button will cycle through. Toy will do nothing on startup, and clicking the button once will run the 'first' pattern
  // Clicking again will run 'second', etc.

  AddPattern(first);
  AddPattern(second);
  AddPattern(third);
  AddPattern(pulse);
  AddPattern(fadeCos);
  AddPattern(weird3);
  AddPattern(sharpRamp);
  AddPattern(flicker);
  AddPattern(weird2);
  AddPattern(fadeOffset);
  AddPattern(pulse2);
  userPatternNumber = currentPatternNumber; // Remember number of Userpattern
  AddPattern(userPatternPlayer);
  AddPattern(allStop);

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

// Serial console. Read a character in to inChar, and a value in to val
void serialProcessor() { 
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    byte val;
    switch (inChar) {
      case 'l': // Set LED-Power
        val = SerialParseInt();
        SetLED(val);
        PrintLine(val);
        break;
        
      case '0': // Set Power for Motor1-3 directly
      case '1':
      case '2':
        val = SerialParseInt();
        SetOutput(inChar, val);
        PrintLine(val);
        break;
        
      case '-':  // Catch '-1', set power of all motors
      {
        int out = SerialParseInt();
        out *= -1;
        val = SerialParseInt();
        SetOutput(out,val);
        break;
      }
      
      case '{': // Set Power for all 3 Motors in a single line
        for (int i = 0; i < 3; i++) {
          val = SerialParseInt();
          if (val >= 0)
            SetOutput(i, val);
        }
        break;
        
      case 'p': // Decrease Toy power, same as longpressing the button
        PrintLine(Toy.decreasePower());
        break;
        
      case 'P': // Increase Toy power, same as doubleclicking the button
        PrintLine(Toy.increasePower());
        break;
        
      case 't': // Decrease pattern time, as in everything goes faster
        PrintLine(Toy.decreaseTime());
        break;
        
      case 'T': // Increase pattern time, as in everything goes slower : Returns new Timescale
        PrintLine(Toy.increaseTime());
        break;
        
      case 'r': // Run a specific pattern, r,0; r,1; r 2 etc.
        val = SerialParseInt();
        Toy.runPattern(val);
        PrintPattern();
        break;
        
      case 'g': // Get number of currently running pattern
        PrintPattern();
        break;
        
      case 's': // Stop all Motors, reset Patternnumber, Power & Timescale
        Toy.stop();
        break;
        
      case 'c': // Cycle Pattern, same as single clicking the button
        Toy.cyclePattern();
        PrintPattern();
        break;
        
      case 'i': // Read input 0 or 1 and print it to serial port
      {
        int in = SerialParseInt();
        in %= Toy.device.inCount;
        PrintLine(Toy.getInput(in));
        break;
      }
      
      case 'v': // Print Version information
        PrintLine(txtVersion);
        break;
        
      case 'G': // Get the Powerlevel
        PrintLine(Toy.getPowerScaleFactor());
        break;
        
      case 'U': // Read User-Pattern
        // UserPattern created before?
        if (userPattern != 0) {
          Toy.stop(); // It's maybe currently playing the User-Pattern, therefore stop
          // Remove the old UserPattern
          delete [] userPattern;
        }
        userPatternSize = SerialParseInt(); // Number of entries
        userPattern = new int[userPatternSize * 4]; // Each entry consists of 3 motorvalues (-1 also possible) and a time
        for (int i = 0; i < userPatternSize*4; i++)
          userPattern[i] = SerialParseInt();

        Toy.runPattern(userPatternNumber);
        PrintPattern();
        break;
        
      case 'u': // Print User-Pattern
        String strPrint = "";
        strPrint += userPatternSize;
        if (userPatternSize != 0) {
          int k = 0;
          for (int i = 0; i < userPatternSize; i++) {
            strPrint += ",";
            for (int j = 0; j < 4; j++) {
              strPrint += userPattern[k++];
              if (j < 3)
                strPrint += " ";
            }
          }
        }
        PrintLine(strPrint);
        break;
    }
  }
}

// Cycle through all the outputs, turn the LED on and leave it on to show that we're on
void startupSequence() {
  int outs = Toy.device.outCount;
  int outInterval = 100 / outs;

  SetLED(128);

  for (int i = 0; i < outs; i++) {
    SetOutput(i, 100+outInterval*i);
    delay(100);
    SetOutput(i, 0);
    delay(50);
  }
  
  for (int i = 0; i < 3; i++) {
    SetLED(0);
    delay(100);
    SetLED(128);
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

// Add a pattern to the toy
void AddPattern(int (*patternFunc)(int)) {
  Toy.addPattern(patternFunc);
  currentPatternNumber++;
}

// Often used object-functions in own functions = smaller
int SerialParseInt() {
  return Serial.parseInt();
}

void SetOutput(int n, int v) {
    Toy.setOutput(n, v);
}

void SetLED(int v) {
    Toy.setLED(0, v);
}

// Serial.println-Calls in functions, this way object-reference only needed once
void PrintLine(float number) {
  Serial.println(number);
}

void PrintLine(int number) {
  Serial.println(number);
}

void PrintLine(unsigned int number) {
  Serial.println(number);
}

void PrintLine(String text) {
  Serial.println(text);
}

// Print Pattern number or "User Pattern"
void PrintPattern() {
  int val = Toy.getPattern();
  if (val == userPatternNumber) {
    String strPrint = "";
    strPrint += val;
    strPrint += " (User Pattern)";
    PrintLine(strPrint);
  }
  else
    PrintLine(val);
}

// Play the User transmitted pattern, for example:
// U 3,255 0 0 500, 0 0 255 500, 0 255 0 500
// 3 Entries
// First Motor 255, Second 0, Third 0, for 500ms
// First Motor 0, Second 0, Third 255, for 500ms
// First Motor 0, Second 255, Third 0, for 500ms
int userPatternPlayer(int seq) {
  // If no Userpattern created yet, skip to next pattern
  if (userPatternSize == 0)
    Toy.cyclePattern();
  else {
    seq %= userPatternSize;
    SetMotorsAndTime(userPattern[seq*4], userPattern[seq*4+1], userPattern[seq*4+2], userPattern[seq*4+3]);
  }
  return 1;
}

