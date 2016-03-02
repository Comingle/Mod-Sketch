/* patterns.h v0.2.5 -- written by Craig Durkin / Comingle. */
/* reworked/comments added by Holger 'Lynxx' Hippenstiel */
#define MOTOR1 0
#define MOTOR2 1
#define MOTOR3 2
#define STEPTIME 3

// Set a motor to a value
void SetMotor(int n, int v) {
  Toy.step[n] = v;
}

// Set the time for a pattern
void SetTime(int v) {
  SetMotor(STEPTIME, v);
}

// Set the same value for all motors
void SetMotors(int v) {
  SetMotor(MOTOR1, v);
  SetMotor(MOTOR2, v);
  SetMotor(MOTOR3, v);
}

// Set the values for all motors and the time
void SetMotorsAndTime(int v1, int v2, int v3, int v4) {
  SetMotor(MOTOR1, v1);
  SetMotor(MOTOR2, v2);
  SetMotor(MOTOR3, v3);
  SetTime(v4);
}

// Begin Pattern functions

// First motor only
// Why have a 50ms timing on the step (Toy.step[3]) ? This lets you adjust the power of the pattern,
// so that instead of running [100, 0, 0, 50] the whole time, it might become [120, 0, 0, 50] after a button click
int first(int seq) {
  SetMotorsAndTime(100, 0, 0, 50);
  return 1;
}

// Second motor only
int second(int seq) {
  SetMotorsAndTime(0, 100, 0, 50);
  return 1;
}

// Third motor only
int third(int seq) {
  SetMotorsAndTime(0, 0, 100, 50);
  return 1;
}

// Turn on all outputs slightly offset from each other.
int flicker(int seq) {
  // set all motors initally to -1, ie "leave it alone"
  SetMotors(-1);

  if (seq > 2)
    SetTime(200);
  else
    SetTime(20);

  seq %= 3;
  SetMotor(seq, 80);

  return 1;
}

// Constantly randomly blip an output on for a short burst.
int pulse(int seq) {
  if (seq % 2)
    SetMotors(0);
  else
    SetMotor(random(0,3), 144);

  SetTime(70);
  return 1;
}

// Opposite of pulse() -- turn on all outputs, randomly blip one off
int pulse2(int seq) {
  if (seq % 2)
    SetMotors(100);
  else
    SetMotor(random(0,3), 0);

  SetTime(100);
  return 1;
}

// Currently unused
int on_off(int seq) {
  if (seq % 20)
    SetMotor(MOTOR1, 200);
  else
    SetMotor(MOTOR1, 0);

  return 1;
}

int weird2(int seq) {
  SetMotor(MOTOR3, round(127*cos(tan(tan(seq/(8*PI)))-PI/2)+127));
  SetTime(30);
  return 1;
}

int weird3(int seq) {
  SetMotor(MOTOR3, round(50*(cos(seq/(8*PI)+PI/2) + sin(seq/2))+100));
  SetTime(30);
  return 1;
}

int sharpRamp(int seq) {
  // neat exponential sequence inspired by github/jgeisler0303
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};

  seq %= 32;
  SetMotors(fadeTable[seq]);

  SetTime(12);
  return 1; 
}

int fadeCos(int seq) {
  SetMotors(round(127 * cos((seq / (8*PI))-PI) + 127));
  SetTime(50);
  return 1;
}

int fadeOffset(int seq) {
  // cos sequence takes 158 steps to run. start motor 1 a third of the way in (53 steps), start motor 2 2/3 (106 steps) of the way in.
  SetMotor(MOTOR1, round(127 * cos((seq / (8*PI))-PI) + 127));
  if (seq >= 58)
    SetMotor(MOTOR2, round(127 * cos(((seq-58) / (8*PI))-PI) + 127));

  if (seq >= 106)
    SetMotor(MOTOR3, round(127 * cos(((seq-106) / (8*PI))-PI) + 127));

  SetTime(50);
  return 1;
}

// This pattern does nothing, execept stop all Motors
int allStop(int seq) {
  SetMotors(0);  
  SetTime(50);
  return 1;
}

