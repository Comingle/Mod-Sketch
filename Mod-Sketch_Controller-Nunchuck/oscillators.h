/*************************************************
 * Oscillator functions
 * These are specially designed
 * for working with the low resolution
 * of digitally controlled motors
 * that give a range of 0-255 to work with
 * ******************************************************
 */

/* Cos Motor
 *   Function for COS that simply takes in values from a constantly increasing iterator (seq)
 *   and outputs a useful value for a motor between 0 and 255
*/
int cosmotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;

  amplitude = amplitude / 2;  // make go between 0-amplitude instead of -1 to 1

  //Basic cos function
  //y(t) = A*cos(2*pi* f *t + phase)   // phase is the point (in radians) where cos is 1
  val = round(amplitude * cos((t / (2 * PI) * freq) - phaseshift) + amplitude);
  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}

/* SquareWave Motor
// Function that simply takes in values from a constantly increasing iterator (seq)
//and outputs a useful value for a motor between that is EITHER 0 and 255
*/
int squaremotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;
  val = round(cos((t / (2 * PI) * freq) - phaseshift) + 1) * amplitude;
  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}


/* Sawtooth Motor
// Function that simply takes in values from a  constantly increasing iterator (seq)
//and outputs a useful value for a motor between 0 and 255
//increases linearly from 0 to 255 and restarts from 0 to 255
*/
int sawmotorOsc(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;
  val = round(t + phaseshift) % round((1 / freq) * amplitude) * freq;

  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}


/* Sawtooth Motor INVERSED
Same as Sawtooth function, but DECREASING
*/
int sawmotorOscINV(int t, int amplitude, float freq, float phaseshift ) {

  int val = -1;
  val = round(t + phaseshift) % round((1 / freq) * amplitude) * freq;

  //Inversion
  val = 255 - val;

  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}



/* SharpRamp Motor
 // neat exponential sequence inspired by github/jgeisler0303
// Function that simply takes in values from a constant iterator (seq)
//and outputs a useful value for a motor between 0 and 255
it does this in an exponentially INCREASING manner for a real sharp attack

*/
int sharpmotorOsc(int t, int amplitude, float freq, float phaseshift) {

  int val = -1;
  //Instead of computing an exponential, it just reads from this table
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  t = round(t + phaseshift) % round((1 / freq) * 32) * freq;

  t %= 32;
  if (t < 32) {
    val = fadeTable[t] * amplitude / 255;
  }
  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}

/* SharpRamp Motor INVERTED
 *Same as SharpRamp Motor, but inverted to be exponentially DECREASING
 */
int sharpmotorOscINV(int t, int amplitude, float freq, float phaseshift) {

  int val = -1;
  //Instead of computing an exponential, it just reads from this table
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  t = round(t + phaseshift) % round((1 / freq) * 32) * freq;

  t %= 32;
  if (t < 32) {
    val = fadeTable[t] * amplitude / 255;
  }

  //Inversion
  val = 255 - val;

  return constrain(val, 0, 255); // motors can't go more than 255 or less than 0
}

/* Virtual Locator
 * Virtually moves the "point of vibration" up and down the shaft
 * interpolated between the 3 motors
 * will return a scaling factor between 0 and 255 given a specific motor number (e.g. 0,1,2),
and a number in a range of 0-1000
 */

int virtualLoc(int val, int motornum) {
  float motorscale = -1;

  //virtual "width" of the point of location
  // a berth of of 1000 for instance would almost always have all 3 motors on
  // a berth of 10 would only trigger the motor closest to where the virtual point is on the shaft
  //a berth of  (#motors-1)/1000 gives a berth 1 motor wide
  //(so when all the way on one motor, others hit the point where they just stop
  float berth = 250;

  //the static virtual positions of each of the motors
  //currently for a device with 3 motors
  //(points evenly spaced at 0,1,2)
  //giving static positions at 0, 500, and 1000
  float motorpos = motornum * 500;

  //  takes a range between 0-1000
  motorscale = abs(motorpos - val); // calculate total distance between motor and value
  motorscale = constrain(berth - motorscale, 0, 1000);// if the distance is greater than the motor berth, don't turn the motor on

  //TODO: see if this mapping makes sense
  motorscale = map(motorscale, 0, berth, 0, 255); // scale the distance to a value usuable by the motor
  return motorscale;
}


/*
 * Perlin Noise Functions reduced for use with motors
 * It's a nice type of noise that is meant to feel more ORGANIC
 * Originally created by Ken Perlin
 * Perlin Noise Code based on perlin function in this source
  http://code.google.com/p/britable/source/browse/trunk/britable/britable.pde#
  http://code.google.com/p/britable/
  kasperkamperman.com 16-09-2012
 */

float lerp(float a, float b, float x)
{ return a + x * (b - a);
}
float perlin_noise_2d(int x, int y) {
  long n = (long)x + (long)y * 57;
  n = (n << 13)^ n;
  return 1.0 - (((n * (n * n * 15731L + 789221L) + 1376312589L)  & 0x7fffffff) / 1073741824.0);
}
float perlin_function(float x, float y)
{
  int fx = floor(x);
  int fy = floor(y);

  float s, t, u, v;
  s = perlin_noise_2d(fx, fy);
  t = perlin_noise_2d(fx + 1, fy);
  u = perlin_noise_2d(fx, fy + 1);
  v = perlin_noise_2d(fx + 1, fy + 1);

  float inter1 = lerp(s, t, x - fx);
  float inter2 = lerp(u, v, x - fx);

  return lerp(inter1, inter2, y - fy);
}

// returns a value between 0 - 255 for lights or motors
float renderNoise(float x, float y)
{
  float noise;

  // 2 octaves
  //noise = perlin_function(x,y) + perlin_function(x*2,y*2);

  noise = perlin_function(x, y); // gives noise in the range of -1 to +1
  noise = noise * 128 + 127;    // scale to a number between 0 - 255

  return noise;
}
/*End perlin funcs*/



/*MULTIMAP
 * This is a handy function for doing non-linear mappings
 * Example From: http://playground.arduino.cc/Main/MultiMap
 * note: the _in array should ALWAYS have increasing values
 * */

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

