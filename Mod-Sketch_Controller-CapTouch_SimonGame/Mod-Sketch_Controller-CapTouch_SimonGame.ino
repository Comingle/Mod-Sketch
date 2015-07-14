/* Mod Sketch v0.2 -- written by Craig Durkin / Comingle. */
/* This software comes pre-loaded on Comingle Mod sex toys */

/* Include the library */
#include <OSSex.h>
#include <ADCTouch.h>
#include <Wire.h> // fickle library you have to include
#include "patterns.h"
#include "simon.h"

int led = 13;

//Cap touch sensing Setup

int ref0, ref1;       //reference values to remove offset
int qvalue0;
int qvalue1;

int controlval0=0;
int controlval1=0;


//Simon Declarations

/*Pin definitions*/
const int programButton = 0;   // Record A New Knock button.
const int ledPin = 13;          // The built in LED
const int knockSensor = 1;     // (Analog 1) for using the piezo as an input device. (aka knock sensor)
const int audioOut = 10;        // (PWM 2) for using the peizo as an output device. (Thing that goes beep.)

/*Tuning constants. Changing the values below changes the behavior of the device.*/
int threshold = 20;                 // Minimum signal from the piezo to register as a knock. Higher = less sensitive. Typical values 1 - 10
const int rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock. Typical values 10-30
const int averageRejectValue = 15; // If the average timing of all the knocks is off by this percent we don't unlock. Typical values 5-20
const int knockFadeTime = 150;     // Milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)
const int lockOperateTime = 2500;  // Milliseconds that we operate the lock solenoid latch before releasing it.
const int maximumKnocks = 20;      // Maximum number of knocks to listen for.
const int knockComplete = 1200;    // Longest time to wait for a knock before we assume that it's finished. (milliseconds)


byte secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."
int knockReadings[maximumKnocks];    // When someone knocks this array fills with the delays between knocks.
int knockSensorValue = 0;            // Last reading of the knock sensor.
boolean programModeActive = false;   // True if we're trying to program a new knock.

int simonLevel=2; //Increase the level to increase the hardness
bool simonRoundStart = false;




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
  Toy.setID(BETA);
  
 // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

//add all our clicks and patterns
  attachClicks();
  addPatterns();


  // Start the Serial console
  Serial.begin(9600);

}



void loop() {
  
 // serialProcessor();
  captouchProcessor();
  
  simpleSimon();

}

int captouchProcessor(){
  
  
   qvalue0 = ADCTouch.read(A2);   //no second parameter
     qvalue1 = ADCTouch.read(A3);     //   --> 100 samples

    qvalue0 -= ref0;       //remove offset
    qvalue1 -= ref1;

    Serial.print(qvalue0 > 40);    //return pressed or not pressed
    Serial.print("\t");
    Serial.print(qvalue1 > 40);
    Serial.print("\t\t");
    Serial.print(qvalue0);             //return value
    Serial.print("\t");
    Serial.println(qvalue1);
    
    controlval0=constrain(qvalue0*4,0,255);
    
            controlval1=constrain(qvalue1*4,0,255);

  //  analogWrite(led,controlval0); 
 //    delay(1); //delay is for the cap touch sensing
  return controlval0;
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
//Toy.addPattern(simpleSimon);
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




/*
        Oscillator functions for low res motor work
************************************************
*/

/* Cos Motor
// Function for COS that simply takes in values from a constant iterator (seq)
//and outputs a useful value for a motor between 0 and 255
*/
int cosmotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;

  amplitude = amplitude / 2;  // make go between 0-amplitude instead of -1 to 1

  //Basic cos function
  //y(t) = A*cos(2*pi* f *t + phase)   // phase is point (in radians) where cos is 1


  // val = round(127 * cos((t / (8*PI))-PI) + 127);
  //val = round(amplitude * cos((t / (8*PI))-phaseshift) + amplitude);

  val = round(amplitude * cos((t / (2 * PI) * freq) - phaseshift) + amplitude);


  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}

/* SquareWave Motor
// Function that simply takes in values from a constant iterator (seq)
//and outputs a useful value for a motor between that is EITHER 0 and 255
*/
int squaremotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;


  // val = round(t + phaseshift) % round((1 / freq) * amplitude) * freq;

  val = round(cos((t / (2 * PI) * freq) - phaseshift) + 1) * amplitude;


  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}



/* Sawtooth Motor
// Function that simply takes in values from a constant iterator (seq)
//and outputs a useful value for a motor between 0 and 255
*/
int sawmotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;


  val = round(t + phaseshift) % round((1 / freq) * amplitude) * freq;


  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}


/* SharpRamp Motor
 // neat exponential sequence inspired by github/jgeisler0303

// Function that simply takes in values from a constant iterator (seq)
//and outputs a useful value for a motor between 0 and 255

*/
int sharpmotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};


  t = round(t + phaseshift) % round((1 / freq) * 32) * freq;


  t %= 32;
  if (t < 32) {
    val = fadeTable[t] * amplitude / 255;
  }


  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}



//will return a scaling factor between 0 and 255 given a specific motor number,
//and a number in a range of 0-1000
int virtualLoc(int val, int motornum) {
  float motorscale = -1;

  float berth = 500;
  float motorpos = motornum * berth;

  //total numbers right now is 1

  //  takes a range between 0-1000

  //current berth is 1 motor wide (so when all the way on one motor, others just stop

  val = map(val, 0, 1000, 0, 2000);


  motorscale = berth - abs(motorpos - val), 0, 1000; // calculate total distance
  motorscale = constrain(motorscale, 0, 1000);
  motorscale = map(motorscale, 0, 1000, 0, 255);
  return motorscale;
}

//////////////////
////PERLIN Noise

/* Perlin Noise Code based on perlin function in this source
  http://code.google.com/p/britable/source/browse/trunk/britable/britable.pde#
  http://code.google.com/p/britable/
  kasperkamperman.com 16-09-2012
*/

float lerp(float a, float b, float x)
{ return a + x * (b - a);
}
float perlin_noise_2d(int x, int y) {
 long n=(long)x+(long)y*57;
 n = (n<<13)^ n;
 return 1.0 - (((n *(n * n * 15731L + 789221L) + 1376312589L)  & 0x7fffffff) / 1073741824.0);    
}
float perlin_function(float x, float y)
{
 int fx = floor(x);
 int fy = floor(y);
 
 float s,t,u,v;
 s=perlin_noise_2d(fx,fy);
 t=perlin_noise_2d(fx+1,fy);
 u=perlin_noise_2d(fx,fy+1);
 v=perlin_noise_2d(fx+1,fy+1);
 
 float inter1 = lerp(s,t,x-fx);
 float inter2 = lerp(u,v,x-fx);  

 return lerp(inter1,inter2,y-fy);
}

// returns a value between 0 - 255 for lights
float renderNoise(float x, float y)
{	
 float noise;
 
 // 2 octaves
 //noise = perlin_function(x,y) + perlin_function(x*2,y*2);
 
 noise = perlin_function(x,y); // gives noise in the range of -1 to +1
 noise = noise *128+127;       // scale to a number between 0 - 255
 	 
 return noise;  
}
//End perlin funcs



/////////////
/////Patterns
////////////
/*
*************** One - off Patterns and clicks
****************
*/
////////////


//Todo !!! , leads to screwups when incorporated directly in the main loop
//global variable for "charging up" the power of the dildo through functions
float chargepow = -1;
bool charging = false;

void charge(){
  
  chargepow=chargepow+0.5;
     Toy.setOutput(0, constrain(chargepow,0,200));
    Toy.setOutput(1, constrain(chargepow-150,0,200));
 Toy.setOutput(2, constrain(chargepow-300,0,200));  
 
 charging = true;
 
 if(chargepow>1000) chargepow=1000;
}

void discharge(){
 
 if(chargepow>0){ 
   
   
  chargepow = chargepow-10;
//  //reverse the order too
//     Toy.setOutput(2, chargepow);
//    Toy.setOutput(1, constrain(chargepow-200,0,255));
// Toy.setOutput(0, constrain(chargepow-400,0,255));  
  // neat exponential sequence inspired by github/jgeisler0303
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

int i = constrain(map(chargepow,1000,0,0,32),0,31);

    Toy.setOutput(0, fadeTable[i]);
    Toy.setOutput(1, fadeTable[constrain(i + 4, 0, 31)]);
    Toy.setOutput(2, fadeTable[constrain(i + 8, 0, 31)]);

  
 
 }
 
 
 else{
     if(charging){Toy.setOutput(-1, 0);
     charging=false; //fully discharged, stop stopping it!
     }
 }
 
}

void blaster() { //cicada like
  // neat exponential sequence inspired by github/jgeisler0303
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  for (int i = 0; i < 32; i++) {
    Toy.setOutput(-1, fadeTable[i]);
    delay(constrain(map(controlval0, 0, 255, 3, 20), 3, 20));
  }
  Toy.setOutput(-1, 0);


}
void chargeblaster() {
  // neat exponential sequence inspired by github/jgeisler0303
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  for (int i = 0; i < 32; i++) {
    Toy.setOutput(0, fadeTable[i]);
    Toy.setOutput(1, fadeTable[constrain(i + 4, 0, 31)]);
    Toy.setOutput(2, fadeTable[constrain(i + 8, 0, 31)]);

    delay(constrain(map(controlval0, 0, 255, 3, 20), 3, 20));
  }
  Toy.setOutput(-1, 0);


}



/*
       Begin Pattern functions
************************************************
*/


//Just turn the motors on and off
int on_off(int seq) {

  int period = constrain(map(controlval0, 0, 255, 1, 100),1,100);
  int power = constrain(map(controlval1, 0, 255, 3, 255), 3, 255);

  if (seq % 2) {
    Toy.step[0] = power;
  } else {
    Toy.step[0] = 0;
  }
  Toy.step[3] = period; // This can be a variable in others
  return 1;
}

//FLAGGED
//Something wrong with this
int movemotor(int seq) {
int location = 0;
//  location = map(nunchuck.readPitch(), 30, 130, 0, 1000); // 0-1000 0 is base motor, 500 is middle, 1000 is tip
 // location = map(nunchuck.readRoll(), -70, 70, 0, 1000); // 0-1000 0 is base motor, 500 is middle, 1000 is tip
 location = map(controlval0, 0, 255, 0, 1000);

          Toy.step[0] = virtualLoc( location, 0);
  Toy.step[1] = virtualLoc( location, 1);
  Toy.step[2] = virtualLoc( location, 2);
  Toy.step[3] = 20;
  return 1;
}



// First motor only
int first(int seq) {

  int power = controlval0;

  Toy.step[0] = power;
  Toy.step[1] = 0;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}



// Second motor only
int second(int seq) {

  int power = controlval0;

  Toy.step[0] = 0;
  Toy.step[1] = power;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}

// Third motor only
int third(int seq) {
  int power = controlval0;
  Toy.step[0] = 0;
  Toy.step[1] = 0;
  Toy.step[2] = power;
  Toy.step[3] = 50;
  return 1;
}

// Turn on all outputs slightly offset from each other.
int flicker(int seq) {
  // reset all motors initally to -1, ie "leave it alone"
  Toy.step[0] = Toy.step[1] = Toy.step[2] = -1;

  if (seq > 2) {
    Toy.step[3] = 200;
  } else {
    Toy.step[3] = 20;
  }

  seq %= 3;
  Toy.step[seq] = controlval0;

  return 1;
}


// Constant Randomly blip an output on for a standard length short burst.
int pulse(int seq) {

  int power = controlval0;
  int duration = controlval1;

  if (seq % 2) { // every other sequence flip it on and off
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;
  } else {
    Toy.step[random(0, 3)] = power;
  }

  Toy.step[3] = duration;
  return 1;
}

// Opposite of the first pulse() -- turn on all outputs, randomly blip one off
int pulseinv(int seq) {
  
    int power = controlval0;
  int duration = controlval1;
  if (seq % 2) {
    Toy.step[0] = Toy.step[1] = Toy.step[2] = power;
  } else {
    Toy.step[random(0, 3)] = 0;
  }

  Toy.step[3] = duration;



  return 1;
}

//Fully random pattern, random motor is chosen, for random periods of time and duration
int randomBlip(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = controlval0;

  //Reset all motors to 0

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;

  Toy.step[0] = random() % 2 * power;
  Toy.step[1] = random() % 2 * power;
  Toy.step[2] = random() % 2 * power;


  Toy.step[3] = random(mintime, mintime + timespread);

  return 1;

}

//Randomblip, but only 1 motor at a time
int singleRandomBlip(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = controlval0;

  //Reset all motors to 0
  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;

  // randomly multiples the power by a 0 or 1
  Toy.step[random(0, 3)] = random() % 2 * power;

  Toy.step[3] = random(mintime, mintime + timespread);
  return 1;

}

//Inverted SingleRandomblip, but only 1 motor OFF at a time
int singleRandomBlipinv(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = controlval0;

  //Reset all motors to 0
  Toy.step[0] = Toy.step[1] = Toy.step[2] = power;

  // randomly multiples the power by a 0 or 1
  Toy.step[random(0, 3)] = random() % 2 * power;

  Toy.step[3] = random(mintime, mintime + timespread);

  return 1;

}




//exponential increase and drop
int sharpRamp(int seq) {
  int amp = controlval0;
  float freq = constrain(map(controlval1,0,255, 1000,1),1,1000)/1000;
  float phaseshift = 0; // between 0 and 255

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

//exponential increase and drop
int cicada(int seq) {
  int amp = 255;
  float freq = constrain(map(controlval1,0,255, 1000,1),1,1000)/1000;
  float phaseshift = controlval0; // between 0 and 255

  Toy.step[0] = 255 - sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = 255 - sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = 255 - sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

// The Dougaller
//A sharp ramp that drifts in and out of phase with itself
int dougaller(int seq) {
  int amp = controlval0;
  float freq = .4;

  float phasefreq = .01;
  float phaseshift = controlval0;

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

  return 1;
}



int fadeCos(int seq) {

  int amp = 255;
  float freq = .25;
  float phaseshift = map(controlval0, 0, 255, PI, 0);

  Toy.step[0] = cosmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}


int shakeFlow(int seq) {
  //decent values between 5 and 50
  int decay = map(controlval1, 0, 255, 100, 5);
  chargepow = constrain(chargepow+controlval0 -decay,0,4000);

  float amp = chargepow/10;
  float freq = .5+ chargepow/1000;
  float phaseshift = 0;
//  Serial.println("   AMP  ");

//Serial.println(ampint);
  Toy.step[0] = constrain(amp-50,0,255);  // send them below 0 to get rid of noise

  Toy.step[1] = constrain(amp-100,0,255);

  Toy.step[2] = constrain(amp-150,0,255);

  Toy.step[3] = 10;
  return 1;
}


int shakeWave(int seq) {
  
  int decay = map(controlval1, 0, 255, 100, 5);
  chargepow = constrain(chargepow+controlval0 -decay,0,3000);

  int amp = chargepow/10;
  float freq = .5+ chargepow/1000;
  float phaseshift = 0;

  Toy.step[0] = cosmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

int shiftingWaves(int seq) {

  int amp = controlval0;
  float freq = .4;

  float phaseshift = 0;
  float  phasefreq = .01;
  float freqshift = cosmotorOsc(seq, 255, phasefreq, 0); // between 0 and 255

  Toy.step[0] = cosmotorOsc(seq, amp, freq + freqshift / 255, 0);

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

}


int fadeSaw(int seq) {

  int amp = controlval1;
  float freq = 4;
  float phaseshift = controlval0; // between 0 and 255

  Toy.step[0] = sawmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sawmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sawmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}



//
//Rain-like splattering with random noise
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforest(int seq) {

  int rainyness = controlval0;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

  Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = random(10, 20) ; // set standard time for a rainblob to hit



  return 1;
}



//The storms cycle in and out  Rain-like splattering
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforeststorm(int seq) {

  int amp = 255;

  float  phasefreq = constrain(map(controlval0, 0, 255, 10, 4000)/1000, .01, 4);
  float freqshift = cosmotorOsc(seq, amp, phasefreq, PI); // between 0 and 255
  int rainyness = freqshift ;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors



  Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = random(10, 20) ; // set standard time for a rainblob to hit



  return 1;
}

//thumper
//Undulates the back and middle, while tapping the front
int thumper(int seq) {

  int amp = 100;

  int thumpamp = controlval1;

  float thumpfreq = constrain(map(controlval0, 0, 255, 100, 8000)/1000, .01, 10);

  float freq = .4;

  float phaseshift = 0;


  Toy.step[0] = squaremotorOsc(seq, thumpamp, thumpfreq, phaseshift );

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

  return 1;
}


//Mostly Harmless by Anonymous
/* Multistage - Longer form pattern
Could the pattern to be something that starts out mellow and pleasant,
then it does a bit of a dance,

works up to a crescendo.
*/
int mostlyHarmless(int seq) {

  int amp = 255;
  float freq = .1;
  float phaseshift = 100; // between 0 and 255

  float time = 10;


  //starts out mellow and pleasant,

  if (seq < 5000) {

    Toy.step[0] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

    Toy.step[1] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

    Toy.step[2] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

  }


  //then it does a bit of a dance
  //Dougaller in the middle

  if (seq > 5000 &&
      seq < 7000) {
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;
    amp = 255;
    freq = .9;

    float phasefreq = .01;
    phaseshift = cosmotorOsc(seq, 255, phasefreq, 0); // between 0 and 255

    Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

    Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

    Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  }



  //works up to a crescendo.
  if (seq > 7000) {
    Toy.step[0] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

    Toy.step[1] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

    Toy.step[2] = cosmotorOsc(seq, 5 + seq / 300, freq + .3 * seq / 10000, 0);

  }



  Toy.step[3] = 10;
  return 1;
}

//Semi-coherent randomness surging through the shaft splattering with Perlin noise
//Some people want bees in the their butt :)
int perlinSwarm(int seq) {

int swarmyness = controlval0;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

Toy.step[0]  = renderNoise(0, seq)-swarmyness;
Toy.step[1]  = renderNoise(1, seq)-swarmyness;
Toy.step[2]  = renderNoise(2, seq)-swarmyness;


  //Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = 10 ; // set standard time for a rainblob to hit

  return 1;
}

//Semi-coherent randomness surging through the shaft splattering with Perlin noise
//Some people want bees in the their butt :)
//This one i try to mess with the time
int perlinSwarmTime(int seq) {

  int swarmyness = controlval0;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

Toy.step[0]  = renderNoise(0, seq);
Toy.step[1]  = renderNoise(1, seq);
Toy.step[2]  = renderNoise(2, seq);

  Toy.step[3] = swarmyness ; // set standard time for a rainblob to hit
  return 1;
}


//////////////////////
////////////////////
//////////////////////////
///////////////////////////
//////////////////////////////
//////////////////////////////
/* one button simon says

Code for creating and detecting patterns with dildos as a game

   partially Adapted from Steve Hoefer http://grathio.com and adafruit knock detector 
   https://learn.adafruit.com/secret-knock-activated-drawer-lock/code

  
*/

// Plays a non-musical tone on the piezo.
// playTime = milliseconds to play the tone
// delayTime = time in microseconds between ticks. (smaller=higher pitch tone.)
void chirp(int playTime, int delayTime){
  long loopTime = (playTime * 1000L) / delayTime;
  pinMode(audioOut, OUTPUT);
  for(int i=0; i < loopTime; i++){
    digitalWrite(audioOut, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(audioOut, LOW);
  }
  pinMode(audioOut, INPUT);
}


// Deals with the knock delay thingy.
void knockDelay(){
  int itterations = (knockFadeTime / 20);      // Wait for the peak to dissipate before listening to next one.
  for (int i=0; i < itterations; i++){
    delay(10);
   // analogRead(knockSensor);                  // This is done in an attempt to defuse the analog sensor's capacitor that will give false readings on high impedance sensors.
    delay(10);
  } 
}



// Plays back the pattern of the knock in blinks and beeps
void playbackKnock(int maxKnockInterval){
      digitalWrite(ledPin, LOW);
      delay(1000);
      digitalWrite(ledPin, HIGH);
      chirp(200, 1800);
      for (int i = 0; i < maximumKnocks ; i++){
        digitalWrite(ledPin, LOW);
        // only turn it on if there's a delay
        if (secretCode[i] > 0){                                   
          delay(map(secretCode[i], 0, 100, 0, maxKnockInterval)); // Expand the time back out to what it was. Roughly. 
          digitalWrite(ledPin, HIGH);
          chirp(200, 1800);
        }
      }
      digitalWrite(ledPin, LOW);
}


//Creates a new random pattern based on a level you input
//higher levels == trickier patterns
void generatePattern(int level){

  //Modify this
//  byte secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."
for (int i = 0; i<maximumKnocks; i++){
  if(i<level){
      secretCode[i]= round(random(1,4))*25;
  }
  else{
   secretCode[i]=0; 
  }
}
  
}







// Records the timing of knocks.
void listenToSecretKnock(){
  int i = 0;
  // First reset the listening array.
  for (i=0; i < maximumKnocks; i++){
    knockReadings[i] = 0;
  }
  
  int currentKnockNumber = 0;               // Position counter for the array.
  int startTime = millis();                 // Reference for when this knock started.
  int now = millis();   

  do {                                      // Listen for the next knock or wait for it to timeout. 
    knockSensorValue = captouchProcessor();
    if (knockSensorValue >= threshold){                   // Here's another knock. Save the time between knocks.
      now=millis();
      knockReadings[currentKnockNumber] = now - startTime;
      currentKnockNumber ++;                             
      startTime = now;          

       if (programModeActive==true){     // Blink the LED when we sense a knock.
         digitalWrite(ledPin, LOW);
       } else {
         digitalWrite(ledPin, HIGH);
       } 
       knockDelay();
       if (programModeActive == true){  // Un-blink the LED.
         digitalWrite(ledPin, HIGH);
       } else {
         digitalWrite(ledPin, LOW);
       }
    }

    now = millis();
  
    // Stop listening if there are too many knocks or there is too much time between knocks.
  } while ((now-startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  
  //we've got our knock recorded, lets see if it's valid
    if (validateKnock() == true){
      Serial.println("you win!");
      simonRoundStart=false;

//They Win! Go to next level
for (i=0; i < 2; i++){          
        digitalWrite(11, HIGH);
        delay(50);
        digitalWrite(11, LOW);
        delay(50);
      }
      
       Toy.setOutput(0,150);
      delay(300);
       Toy.setOutput(0,200);
      delay(300);
       Toy.setOutput(0,255);
      delay(300);
       Toy.setOutput(0,0);
      delay(300);
//They Win! 
    } else {
            Serial.println("you LOSE!");

            simonRoundStart=false;

      Toy.setOutput(0,250);
      delay(300);
       Toy.setOutput(0,150);
      delay(300);
       Toy.setOutput(0,0);
      delay(300);
      
    }
  
}




// Checks to see if our knock matches the secret.
// Returns true if it's a good knock, false if it's not.
boolean validateKnock(){
  int i = 0;
 
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;               // We use this later to normalize the times.
  
  for (i=0;i<maximumKnocks;i++){
    if (knockReadings[i] > 0){
      currentKnockCount++;
    }
    if (secretCode[i] > 0){         
      secretKnockCount++;
    }
    
    if (knockReadings[i] > maxKnockInterval){   // Collect normalization data while we're looping.
      maxKnockInterval = knockReadings[i];
    }
  }
  
 
  if (currentKnockCount != secretKnockCount){  // Easiest check first. If the number of knocks is wrong, don't unlock.
    return false;
  }
  
  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
      (ie: if you do the same pattern slow or fast it should still open the door.)
      This makes it less picky, which while making it less secure can also make it
      less of a pain to use if you're tempo is a little slow or fast. 
  */
  int totaltimeDifferences = 0;
  int timeDiff = 0;
  for (i=0; i < maximumKnocks; i++){    // Normalize the times
    knockReadings[i]= map(knockReadings[i], 0, maxKnockInterval, 0, 100);      
    timeDiff = abs(knockReadings[i] - secretCode[i]);
    if (timeDiff > rejectValue){        // Individual value too far out of whack. No access for this knock!
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences / secretKnockCount > averageRejectValue){
    return false; 
  }
  
  return true;
}


void listenForFirstKnock(){
  
   knockSensorValue = captouchProcessor();
   Serial.println(knockSensorValue);
  if (knockSensorValue >= threshold){
     listenToSecretKnock();           // We have our first knock. Go and see what other knocks are in store...
  }
  
}

int simpleSimon() {
  

  
  //Play the current knock
  if(simonRoundStart==false){
      //generate a new pattern
  generatePattern(simonLevel);
  
      playbackKnock(300);
simonRoundStart=true;
delay (1000);

  }
  
  listenForFirstKnock();
  
return 1;
  
}
