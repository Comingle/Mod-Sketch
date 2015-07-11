/* Cos Motor
// Function for COS that simply takes in values from a constant iterator (seq) 
//and outputs a useful value for a motor between 0 and 255
*/
int cosmotorOsc(int t,int amplitude, float freq, float phaseshift ){
  
  int val=-1;
  
  amplitude = amplitude / 2;  // make go between 0-amplitude instead of -1 to 1
  
 //Basic cos function
  //y(t) = A*cos(2*pi* f *t + phase)   // phase is point (in radians) where cos is 1
  
 
 // val = round(127 * cos((t / (8*PI))-PI) + 127);
  //val = round(amplitude * cos((t / (8*PI))-phaseshift) + amplitude);
  
  val = round(amplitude * cos((t / (2*PI)*freq)-phaseshift) + amplitude);

 
return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}




/* Sawtooth Motor
// Function that simply takes in values from a constant iterator (seq) 
//and outputs a useful value for a motor between 0 and 255
*/
int sawmotorOsc(int t,int amplitude, float freq, float phaseshift ){
  
  int val=-1;


 val = round(t+phaseshift)%round((1/freq)*amplitude)*freq;
  
  
return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}


/* SharpRamp Motor
 // neat exponential sequence inspired by github/jgeisler0303

// Function that simply takes in values from a constant iterator (seq) 
//and outputs a useful value for a motor between 0 and 255

*/
int sharpmotorOsc(int t,int amplitude, float freq, float phaseshift ){
  
  int val=-1;
 const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};


 t = round(t+phaseshift)%round((1/freq)*32)*freq;


  t %= 32;
  if (t < 32) {
    val = fadeTable[t]*amplitude/255;
  }
  
  
return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}




//********
// Begin Pattern functions
//*******
int on_off(int seq) {
  
  int freq = 50;
  
  if (seq % 2) {
    Toy.step[0] = 255;
  } else {
    Toy.step[0] = 0;
  }
  Toy.step[3]=freq; // This can be a variable in others
  return 1;
}


// The Dougaller
int dougaller(int seq) {
  
  
 return 1; 
}


// First motor only
// Why have a 50ms timing on the step (Toy.step[3]) ? This lets you adjust the power of the pattern,
// so that instead of running [100, 0, 0, 50] the whole time, it might become [120, 0, 0, 50] after a button click
int first(int seq) {
  
  int power = 100;
  
  Toy.step[0] = power;
  Toy.step[1] = 0;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}



// Second motor only
int second(int seq) {
  
    int power = 100;
  
  Toy.step[0] = 0;
  Toy.step[1] = power;
  Toy.step[2] = 0;
  Toy.step[3] = 50;
  return 1;
}

// Third motor only
int third(int seq) {
    int power = 100;
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
  Toy.step[seq] = 80;

  return 1;
}


// Constant Randomly blip an output on for a standard length short burst.
int pulse(int seq) {
  
    int power = 100;
      int duration = 70;
  
  if (seq % 2) { // every other sequence flip it on and off
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 0;
  } else {
    Toy.step[random(0,3)] = power;
  }

  Toy.step[3] = duration;
  return 1;
}

//Fully random pattern, random motor is chosen, for random periods of time and duration
int randomBlip(int seq) {
  
  int timespread = 110;
  int mintime = 20;
  int power = 100;
  
//Reset all motors to 0

Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; 

  Toy.step[0] = random()%2*power;
    Toy.step[1] = random()%2*power;
  Toy.step[2] = random()%2*power;

 
  Toy.step[3] = random(mintime,mintime+timespread);

return 1;

}

//Randomblip, but only 1 motor at a time
int singleRandomBlip(int seq) {
  
  int timespread = 110;
  int mintime = 20;
  int power = 130;
  
//Reset all motors to 0
Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; 

// randomly multiples the power by a 0 or 1
  Toy.step[random(0,3)] = random()%2*power;  
 
  Toy.step[3] = random(mintime,mintime+timespread);

return 1;

}

//Inverted SingleRandomblip, but only 1 motor OFF at a time
int invSingleRandomBlip(int seq) {
  
  int timespread = 110;
  int mintime = 20;
  int power = 130;
  
//Reset all motors to 0
Toy.step[0] = Toy.step[1] = Toy.step[2] = power; 

// randomly multiples the power by a 0 or 1
  Toy.step[random(0,3)] = random()%2*power;  
 
  Toy.step[3] = random(mintime,mintime+timespread);

return 1;

}

// Andy Test Rain
int andyRain(int seq) {

Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

//Decide if to drop a raindrop or not

if(!random(
4
)
  
  ){ // Every now and then will return a 0, which we add a ! to to make the statement true
Toy.step[random(0,3)] = 100; // drop a rainblob of standard impact strength
Toy.step[3] = 20; // set standard time for a rainblob to hit
}
else{
 
  Toy.step[3] = random(1,150); //random amount of time between drops
}
  
  return 1;
}



// Raindrops auto-cycling a storm
int andyRainCycle(int seq) {

Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors

//Decide if to drop a raindrop or not

if(!random(
  
  round(5 * cos((seq / (100*PI)))-PI))
  
  ){ // Every now and then will return a 0, which we add a ! to to make the statement true
Toy.step[random(0,3)] = 100; // drop a rainblob of standard impact strength
Toy.step[3] = 60; // set standard time for a rainblob to hit
}
else{
 
  Toy.step[3] = random(20,150); //random amount of time between drops
}
  
  return 1;
}

// Opposite of the first pulse() -- turn on all outputs, randomly blip one off
int pulse2(int seq) {
  if (seq % 2) {
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 100;
  } else {
    Toy.step[random(0,3)] = 0;
  }

  Toy.step[3] = 100;

  
  
  return 1;
}


int weird2(int seq) {
  Toy.step[2] = round(127*cos(tan(tan(seq/(8*PI)))-PI/2)+127);
  Toy.step[3] = 30;
  return 1;
}

int weird3(int seq) {
  Toy.step[2] = round(50*(cos(seq/(8*PI)+PI/2) + sin(seq/2))+100);
  Toy.step[3] = 30;
  return 1;
}


//exponential increase and drop
int sharpRamp(int seq) {
   int amp = 255;
  float freq = .4;
  float phaseshift = 0; // between 0 and 255
  
  Toy.step[0] = sharpmotorOsc(seq,amp, freq, 0);
  
  Toy.step[1] = sharpmotorOsc(seq,amp, freq, phaseshift);
  
  Toy.step[2] = sharpmotorOsc(seq,amp, freq, phaseshift*2);
  
  Toy.step[3] = 10;
  return 1;
}

//exponential increase and drop
int cicada(int seq) {
   int amp = 255;
  float freq = .1;
  float phaseshift = 0; // between 0 and 255
  
  Toy.step[0] =255- sharpmotorOsc(seq,amp, freq, 0);
  
  Toy.step[1] = 255-sharpmotorOsc(seq,amp, freq, phaseshift);
  
  Toy.step[2] = 255-sharpmotorOsc(seq,amp, freq, phaseshift*2);
  
  Toy.step[3] = 10;
  return 1;
}




int fadeCos(int seq) {
  
  int amp = 255;
  float freq = .25;
  float phaseshift = 0;
  
  Toy.step[0] = cosmotorOsc(seq,amp, freq, 0);
  
  Toy.step[1] = cosmotorOsc(seq,amp, freq, phaseshift);
  
  Toy.step[2] = cosmotorOsc(seq,amp, freq, phaseshift*2);
  
  Toy.step[3] = 10;
  return 1;
}



int fadeOffset(int seq) {
  // cos sequence takes 158 steps to run. start motor 1 a third of the way in (53 steps), start motor 2 2/3 (106 steps) of the way in.
  Toy.step[0] = round(127 * cos((seq / (8*PI))-PI) + 127);
  if (seq >= 58) {
    Toy.step[1] = round(127 * cos(((seq-58) / (8*PI))-PI) + 127);
  }
  if (seq >= 106) {
    Toy.step[2] = round(127 * cos(((seq-106) / (8*PI))-PI) + 127);
  }
  Toy.step[3] = 50;
  return 1;
}


int fadeSaw(int seq) {
  
  int amp = 255;
  float freq = 4;
  float phaseshift = 100; // between 0 and 255
  
  Toy.step[0] = sawmotorOsc(seq,amp, freq, 0);
  
  Toy.step[1] = sawmotorOsc(seq,amp, freq, phaseshift);
  
  Toy.step[2] = sawmotorOsc(seq,amp, freq, phaseshift*2);
  
  Toy.step[3] = 10;
  return 1;
}



