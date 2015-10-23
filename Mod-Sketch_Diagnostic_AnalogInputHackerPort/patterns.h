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
    Toy.step[1] = power;


  } else {
  
    Toy.step[0] = 0;
        Toy.step[1] = 0;

  }
  Toy.step[3] = period; // This can be a variable in others
  return 1;
}
//Just turn the motors on and off
int alternate(int seq) {

  int period = constrain(map(controlval0, 0, 255, 1, 100),1,100);
  int power = constrain(map(controlval1, 0, 255, 3, 255), 3, 255);

  if (seq % 2) {
    Toy.step[0] = power;
        Toy.step[1] = 0;

  } else {
         Toy.step[1] = power;
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
  float freq = .4+controlval1/100;
  float phaseshift = 0; // between 0 and 255

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

//exponential increase and drop
int cicada(int seq) {
  int amp = controlval0;
  float freq = .6+controlval1/100;
  float phaseshift = 0;

  Toy.step[0] = amp - sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = amp - sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = amp - sharpmotorOsc(seq, amp, freq, phaseshift * 2);

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

  int amp = controlval0;
  float freq = 4;
  float phaseshift = controlval1; // between 0 and 255

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

Toy.step[0]  = -renderNoise(0, seq)+swarmyness;
Toy.step[1]  = -renderNoise(1, seq)+swarmyness;
Toy.step[2]  = -renderNoise(2, seq)+swarmyness;


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


