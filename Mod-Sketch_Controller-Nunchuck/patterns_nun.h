/*PATTERN FUNCTIONS
 * This file is a compendium of many basic pattern functions
 * that we have written
 * They range from SUPER BASIC (like on_off function)
 * To more sophisticated ones that use special algorithms in the oscillator functions
 *
 * Each function has a special integer variable called seq, which is CONSTANTLY INCREASING
 * You can feed this into different functions to get different behaviors.
 *
 * In sketches where there is a controller (like using the nunchuck)
 * You can see that you can incorporate dynamic variables (sensor readings)
 * that can change how these patterns operate
*/





/*
       Begin Pattern functions
************************************************
*/

/* ON OFF
 * //Just turn the motors on and off
 * one can change the power or frequency
 */

int on_off(int seq) {

  int period = constrain(map(nunchuck.readRoll(), -70, 70, 1, 100), 1, 100);
  int power = constrain(map(nunchuck.readJoyY(), 0, 255, 3, 255), 3, 255);

  if (seq % 2) {
    Toy.step[0] = power;
  } else {
    Toy.step[0] = 0;
  }
  Toy.step[3] = period; // This can be a variable in others
  return 1;
}

/* Single Motor Examples
 *  Just turning them on
 *  First motor only
 *  Just turns on the first motor
Example Question: if we have a constantly ON pattern,
then, Why have a 50ms timing on the step (Toy.step[3]) ?
Answer: This lets you adjust the power of the pattern,
so that instead of running [100, 0, 0, 50] the whole time,
it might become [120, 0, 0, 50] after a button click
 */


// First motor only
int first(int seq) {

  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);

  Toy.step[0] = power;
  Toy.step[1] = 0;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}

// Second motor only
int second(int seq) {

  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);

  Toy.step[0] = 0;
  Toy.step[1] = power;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}

// Third motor only
int third(int seq) {
  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);
  Toy.step[0] = 0;
  Toy.step[1] = 0;
  Toy.step[2] = power;
  Toy.step[3] = 50;
  return 1;
}

/* Onetwothree
like the startup sequence but a constant pattern
*/
int onetwothree(int seq) {

  int power = 255;
  int dur = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 1, 255), 1, 255);

  seq %= 6;

  if (seq == 0 || seq == 2 || seq == 4) {

    Toy.step[0] = 0;
    Toy.step[1] = 0;
    Toy.step[2] = 0;
    Toy.step[3] = dur;
    return 1;

  }

  if (seq == 1) {
    Toy.step[0] = power;
    Toy.step[1] = 0;
    Toy.step[2] = 0;
    Toy.step[3] = dur;
    return 1;

  }

  if (seq == 3) {
    Toy.step[0] = 0;
    Toy.step[1] = power;
    Toy.step[2] = 0;
    Toy.step[3] = dur;
    return 1;

  }


  if (seq == 5) {
    Toy.step[0] = 0;
    Toy.step[1] = 0;
    Toy.step[2] = power;
    Toy.step[3] = dur;
    return 1;

  }
}

/*
 * FLICKER
 * // Turn on all outputs slightly offset from each other.
 */
int flicker(int seq) {
  // reset all motors initally to -1, ie "leave it alone"
  Toy.step[0] = Toy.step[1] = Toy.step[2] = -1;

  if (seq > 2) {
    Toy.step[3] = 200;
  } else {
    Toy.step[3] = 20;
  }

  seq %= 3;
  Toy.step[seq] = constrain( map( abs(nunchuck.readAccelY()), 0, 255, 0, 255), 0, 255);

  return 1;
}



/******
Undulating Wave Functions
******/

int fadeCos(int seq) {

  int amp = 255;
  float freq = .25;
  float phaseshift = map(nunchuck.readJoyY(), 0, 255, PI, 0);
  Toy.step[0] = cosmotorOsc(seq, amp, freq, 0);
  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);
  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);
  Toy.step[3] = 10;
  return 1;
}

int shiftingWaves(int seq) {

  int amp = 255;
  float freq = .4;

  float phaseshift = 0;
  float  phasefreq = .01;
  float freqshift = cosmotorOsc(seq, 255, phasefreq, 0); // between 0 and 255

  Toy.step[0] = cosmotorOsc(seq, amp, freq + freqshift / 255, 0);

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

}


int fadeOffset(int seq) {
  // cos sequence takes 158 steps to run. start motor 1 a third of the way in (53 steps), start motor 2 2/3 (106 steps) of the way in.
  Toy.step[0] = round(127 * cos((seq / (8 * PI)) - PI) + 127);
  if (seq >= 58) {
    Toy.step[1] = round(127 * cos(((seq - 58) / (8 * PI)) - PI) + 127);
  }
  if (seq >= 106) {
    Toy.step[2] = round(127 * cos(((seq - 106) / (8 * PI)) - PI) + 127);
  }
  Toy.step[3] = 50;
  return 1;
}

//exponential increase and drop
int sharpRamp(int seq) {
  int amp = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255);
  float freq = .4 ; //+constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255)/100;
  float phaseshift = 0; // between 0 and 255

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

//exponential increase and drop
int cicada(int seq) {
  int amp = 255; // constrain( map( abs(nunchuck.readAccelY()),0,255,0,255), 0,255);
  float freq = .6 + constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255) / 100;
  float phaseshift = 0;

  Toy.step[0] = amp - sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = amp - sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = amp - sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
  return 1;
}

// The Dougaller
//A sharp ramp that drifts in and out of phase with itself
int dougaller(int seq) {
  int amp = 255;
  float freq = .4;

  float phasefreq = .01;
  float phaseshift = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255);
  // cosmotorOsc(seq, 255, phasefreq, 0); // between 0 and 255

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

  return 1;
}

//Set of weird shifty cosine and tan patterns made by craig
int weird2(int seq) {
  Toy.step[2] = round(127 * cos(tan(tan(seq / (8 * PI))) - PI / 2) + 127);
  Toy.step[3] = 30;
  return 1;
}

int weird3(int seq) {
  Toy.step[2] = round(50 * (cos(seq / (8 * PI) + PI / 2) + sin(seq / 2)) + 100);
  Toy.step[3] = 30;
  return 1;
}

int fadeSaw(int seq) {

  int amp = 255;
  float freq = 4;
  float phaseshift = 100; // between 0 and 255

  Toy.step[0] = sawmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sawmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sawmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}




/***
Funky Patterns
-Just extra patterns that do fun stuff
****/

//thumper
//Undulates the back and middle, while tapping the front
int thumper(int seq) {

  int amp = 100;

  int thumpamp = nunchuck.readJoyY();

  float thumpfreq = constrain(map(nunchuck.readJoyX(), 0, 255, 100, 8000) / 1000, .01, 10);

  float freq = constrain(map(nunchuck.readRoll(), -70, 70, 10, 4000) / 1000, .01, 4);

  float phaseshift = 0;




  Toy.step[0] = squaremotorOsc(seq, thumpamp, thumpfreq, phaseshift );

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;

  return 1;
}


/****
Randomy patterns
****/



/*
 * PULSE:
 * Random Motor Blips
 * Spaced by constant durations
 */
int pulse(int seq) {

  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);
  int duration = 70;

  if (seq % 2) { // every other sequence flip it on and off
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;
  } else {
    Toy.step[random(0, 3)] = power;
  }

  Toy.step[3] = duration;
  return 1;
}

/*
 * Pulse Inverted
 * // Opposite of the first pulse() -- turn on all outputs, randomly blip one off
 */
int pulseinv(int seq) {

  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);
  int duration = 70;

  if (seq % 2) {
    Toy.step[0] = Toy.step[1] = Toy.step[2] = power;
  } else {
    Toy.step[random(0, 3)] = 0;
  }

  Toy.step[3] = duration;

  return 1;
}

/*
 * Random Blip
 * Fully random pattern
 * random motor(s) are chosen, for random periods of time and duration
 */
int randomBlip(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);

  //Reset all motors to 0

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;

  Toy.step[0] = random() % 2 * power;
  Toy.step[1] = random() % 2 * power;
  Toy.step[2] = random() % 2 * power;


  Toy.step[3] = random(mintime, mintime + timespread);

  return 1;

}

/*
 * Single Random Blip
 * Just like
 * /Randomblip, but only 1 motor at a time
 */
int singleRandomBlip(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);

  //Reset all motors to 0
  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;

  // randomly multiples the power by a 0 or 1
  Toy.step[random(0, 3)] = random() % 2 * power;

  Toy.step[3] = random(mintime, mintime + timespread);
  return 1;

}

/*
 * Inverted SingleRandomblip
 * Just like
 * Randomblip, but only 1 motor at a time
 * and inverted
* that is, but only 1 motor OFF at a time
 */

int singleRandomBlipinv(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);

  //Reset all motors to 0
  Toy.step[0] = Toy.step[1] = Toy.step[2] = power;

  // randomly multiples the power by a 0 or 1
  Toy.step[random(0, 3)] = random() % 2 * power;

  Toy.step[3] = random(mintime, mintime + timespread);

  return 1;
}


//Rainforest
//Rain-like splattering with random noise
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforest(int seq) {

  int rainyness = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255);

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

  Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = random(10, 20) ; // set standard time for a rainblob to hit



  return 1;
}



//The storms cycle in and out  Rain-like splattering
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforeststorm(int seq) {

  int amp = 255;

  float  phasefreq = constrain(map(nunchuck.readRoll(), -70, 70, 10, 4000) / 1000, .01, 4);
  float freqshift = cosmotorOsc(seq, amp, phasefreq, PI); // between 0 and 255
  int rainyness = freqshift ;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors
  
  Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = random(10, 20) ; // set standard time for a rainblob to hit



  return 1;
}


//Semi-coherent randomness surging through the shaft splattering with Perlin noise
//Some people want bees in the their butt :)
int perlinSwarm(int seq) {

  int swarmyness = constrain(map(nunchuck.readRoll(), -70, 70, 0, 255), 0, 255);

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

  Toy.step[0]  = renderNoise(0, seq) - swarmyness;
  Toy.step[1]  = renderNoise(1, seq) - swarmyness;
  Toy.step[2]  = renderNoise(2, seq) - swarmyness;

  //Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = 10 ; // set standard time for a rainblob to hit

  return 1;
}

//Semi-coherent randomness surging through the shaft splattering with Perlin noise
//Some people want bees in the their butt :)
//This one i try to mess with the time
int perlinSwarmTime(int seq) {

  int swarmyness = constrain(map(nunchuck.readRoll(), -70, 70, 1, 200), 1, 200);

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

  Toy.step[0]  = renderNoise(0, seq);
  Toy.step[1]  = renderNoise(1, seq);
  Toy.step[2]  = renderNoise(2, seq);

  //Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = swarmyness ; // set standard time for a rainblob to hit
  return 1;
}

/* Move Motor

*/
//FLAGGED
//Something wrong with this
int movemotor(int seq) {
  int location = 0;
  //  location = map(nunchuck.readPitch(), 30, 130, 0, 1000); // 0-1000 0 is base motor, 500 is middle, 1000 is tip
  // location = map(nunchuck.readRoll(), -70, 70, 0, 1000); // 0-1000 0 is base motor, 500 is middle, 1000 is tip
  location = map(nunchuck.readJoyY(), 0, 255, 0, 1000);

  Toy.step[0] = virtualLoc( location, 0);
  Toy.step[1] = virtualLoc( location, 1);
  Toy.step[2] = virtualLoc( location, 2);

  Toy.step[3] = 20;
  return 1;
}


/*** Charging Functions
functions which rely on some sort of integration of movements to
"dole out" power to things like the motors for instance
*/

//Todo !!! , leads to screwups when incorporated directly in the main loop
//global variable for "charging up" the power of the dildo through functions

float chargepow = -1;
bool charging = false;

void charge() {

  chargepow = chargepow + 0.5;
  Toy.setOutput(0, constrain(chargepow, 0, 200));
  Toy.setOutput(1, constrain(chargepow - 150, 0, 200));
  Toy.setOutput(2, constrain(chargepow - 300, 0, 200));

  charging = true;

  if (chargepow > 1000) chargepow = 1000;
}

void discharge() {

  if (chargepow > 0) {


    chargepow = chargepow - 10;
    //  //reverse the order too
    //     Toy.setOutput(2, chargepow);
    //    Toy.setOutput(1, constrain(chargepow-200,0,255));
    // Toy.setOutput(0, constrain(chargepow-400,0,255));
    // neat exponential sequence inspired by github/jgeisler0303
    const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

    int i = constrain(map(chargepow, 1000, 0, 0, 32), 0, 31);

    Toy.setOutput(0, fadeTable[i]);
    Toy.setOutput(1, fadeTable[constrain(i + 4, 0, 31)]);
    Toy.setOutput(2, fadeTable[constrain(i + 8, 0, 31)]);

    //delay(constrain(map(nunchuck.readJoyY(), 0, 255, 3, 20), 3, 20));


  }


  else {
    if (charging) {
      Toy.setOutput(-1, 0);
      charging = false; //fully discharged, stop stopping it!
    }
  }

}

void blaster() { //cicada like
  // neat exponential sequence inspired by github/jgeisler0303
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  for (int i = 0; i < 32; i++) {
    Toy.setOutput(-1, fadeTable[i]);
    delay(constrain(map(nunchuck.readJoyY(), 0, 255, 3, 20), 3, 20));
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

    delay(constrain(map(nunchuck.readJoyY(), 0, 255, 3, 20), 3, 20));
  }
  Toy.setOutput(-1, 0);


}

//outputs the amount that the controller is shaking scaled to 0-255
int nunShake() {
  int shake = -1;

  //a motionless controller  should have a standard value that it outputs from gravity
  //it hovers around 400, and ranges from 300-500
  int still = 500;

  shake = abs(nunchuck.readAccelX()) + abs(nunchuck.readAccelY()) + abs(nunchuck.readAccelZ()) - still;

  //total shake magnitude reaches 1000 - still on harder shakes
  return constrain(map(shake, 0, 1100 - still, 0, 255), 0, 255);
}

// Total shakeyness controls a motor
int shaker(int seq) {

  Toy.step[0] = nunShake();
  Toy.step[1] = nunShake();
  Toy.step[2] = nunShake();
  Toy.step[3] = 20;
  return 1;
}

//Each Axis's shakeyness controls a motor=
int shakerAxes(int seq) {

  Toy.step[0] = constrain( map( abs(nunchuck.readAccelX()), 0, 400, 0, 255), 0, 255);
  Toy.step[1] = constrain( map( abs(nunchuck.readAccelY()), 0, 400, 0, 255), 0, 255);
  Toy.step[2] = constrain( map( abs(nunchuck.readAccelZ()), 0, 400, 0, 255), 0, 255);
  Toy.step[3] = 20;
  return 1;
}




int shakeFlow(int seq) {
  //decent values between 5 and 50
  int decay = map(nunchuck.readJoyY(), 0, 255, 100, 5);
  chargepow = constrain(chargepow + nunShake() - decay, 0, 4000);

  float amp = chargepow / 10;
  float freq = .5 + chargepow / 1000;
  float phaseshift = 0;
  //  Serial.println("   AMP  ");

  //Serial.println(ampint);
  Toy.step[0] = constrain(amp - 50, 0, 255); // send them below 0 to get rid of noise

  Toy.step[1] = constrain(amp - 100, 0, 255);

  Toy.step[2] = constrain(amp - 150, 0, 255);

  Toy.step[3] = 10;
  return 1;
}


int shakeWave(int seq) {

  int decay = map(nunchuck.readJoyY(), 0, 255, 100, 5);
  chargepow = constrain(chargepow + nunShake() - decay, 0, 3000);

  int amp = chargepow / 10;
  float freq = .5 + chargepow / 1000;
  float phaseshift = 0;

  Toy.step[0] = cosmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = cosmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = cosmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

/*
Long Form Patterns
Patterns that contain a bit more of a narrative
and take upwards of several minutes to complete
some are Multistage with Longer form patterns
*/

/*Mostly Harmless by Anonymous
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

/*Bored but Horny - by HPW



/*Bored but Horny - by HPW
*"Bored but Horney": Continuos succession of low pulses with irregular 
*intervals between them, then somewhat random (Perlin noise) clusters 
of more intense high frequency excitement. No buildup, just constant.
 it should just keep alternating between a set of low pulses 
 (...v......v..v........v........v....v......v..v......v.......)
with occasional clusters (....XXX.....)
so, something like this:
...v......v..v........v........v....v......v..v......v.......XX..............v.....v....v.v........v......XXX........v...v...X........v.....v.......v...........v......XX............
*/


