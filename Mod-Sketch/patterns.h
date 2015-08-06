
/*
       Begin Pattern functions
************************************************
*/


//Just turn the motors on and off
int on_off(int seq) {

  int freq = 50;
  int power = 100;

  if (seq % 2) {
    Toy.step[0] = power;
  } else {
    Toy.step[0] = 0;
  }
  Toy.step[3] = freq; // This can be a variable in others
  return 1;
}





// First motor only
// Why have a 50ms timing on the step (Toy.step[3]) ? 
//This lets you adjust the power of the pattern,
// so that instead of running [100, 0, 0, 50] the whole time, 
//it might become [120, 0, 0, 50] after a button click
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
    Toy.step[random(0, 3)] = power;
  }

  Toy.step[3] = duration;
  return 1;
}

// Opposite of the first pulse() -- turn on all outputs, randomly blip one off
int pulseinv(int seq) {
  if (seq % 2) {
    Toy.step[0] = Toy.step[1] = Toy.step[2] = 100;
  } else {
    Toy.step[random(0, 3)] = 0;
  }

  Toy.step[3] = 100;



  return 1;
}

//Fully random pattern, random motor is chosen, for random periods of time and duration
int randomBlip(int seq) {

  int timespread = 110;
  int mintime = 20;
  int power = 100;

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
  int power = 130;

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
  int power = 130;

  //Reset all motors to 0
  Toy.step[0] = Toy.step[1] = Toy.step[2] = power;

  // randomly multiples the power by a 0 or 1
  Toy.step[random(0, 3)] = random() % 2 * power;

  Toy.step[3] = random(mintime, mintime + timespread);

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


//exponential increase and drop
int sharpRamp(int seq) {
  int amp = 255;
  float freq = .4;
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
  float freq = 1;
  float phaseshift = 0; // between 0 and 255

  Toy.step[0] = 255 - sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = 255 - sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = 255 - sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;
  return 1;
}

// The Dougaller
//A sharp ramp that drifts in and out of phase with itself
int dougaller(int seq) {
  int amp = 255;
  float freq = .4;

  float phasefreq = .01;
  float phaseshift = cosmotorOsc(seq, 255, phasefreq, 0); // between 0 and 255

  Toy.step[0] = sharpmotorOsc(seq, amp, freq, 0);

  Toy.step[1] = sharpmotorOsc(seq, amp, freq, phaseshift);

  Toy.step[2] = sharpmotorOsc(seq, amp, freq, phaseshift * 2);

  Toy.step[3] = 10;


  return 1;
}



int fadeCos(int seq) {

  int amp = 255;
  float freq = .25;
  float phaseshift = 0;

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


//Rain-like splattering
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforest(int seq) {

  int rainyness = 50;

  Toy.step[0] = Toy.step[1] = Toy.step[2] = 0; //Reset all motors



  Toy.step[random(0, 3)] = constrain(map(rainyness, 0, 255, 0, 255), 0, 255); // drop a rainblob of standard impact strength
  Toy.step[3] = random(10, 20) ; // set standard time for a rainblob to hit



  return 1;
}

//The storms cycle in and out  Rain-like splattering
//When it's raining in the forest, all is right in the world - Peter Marting
int rainforeststorm(int seq) {

  int amp = 255;

  float  phasefreq = .02;
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
  
    int thumpamp = 255;

  float thumpfreq = 4;

  float freq = .2;

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

