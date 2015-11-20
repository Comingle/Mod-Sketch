/*
  InputProcessor.h - Library for calibrating sensors hooked to analog inputs.
  InputProcessor is released under the 
  hacked off examples by 
  Copyright 2013 Julian Vidal
  https://github.com/poisa/Calibrator

  and Rob Tillaart
*/

#ifndef InputProcessor_h
#define InputProcessor_h

#include "Arduino.h"

class InputProcessor
{
	public:
	
		
	//	InputProcessor(int thepin);
    InputProcessor(int n);
    ~InputProcessor();
  unsigned int pin;               // onboard pin

      //InputProcessor funcs
      int update();
      int update(int funcval); // if you are not just using analogread, you can just pass an integer directly to it
      

   void calibrate(int duration);
   void calibrate(int duration,int funcval);
    void setValue(int currentValue);
    int scaleValue(int currentValue);
    int scaleValue10bit(int currentValue);
    int getMin();
    int getMax();
    void resetCal();
    
    //Calibration Values
    int min;          //the input's min from the calibration
    int max;          //the input's max from the calibration
    int avg;                // the computed from calibration average
    int STDEV;        //Computed Standard deviation from the calibration
    int rawValue;         //gets the most recent reading from the ADC, or value given
    int scaledValue;      //gets the most recent reading scaled between the min and max, and mapped between 0 and 255
    int lastCal = 0;        //time in milliseconds of the last calibration (to find time since previous calibration do millis()-lastCal

//Useful Values for Quick Access (These might be better as functions, but this can be changed)
    int customThreshold = 127;        //A user defined threshold //  defaults to 127 if not specified
    int AvgDiff;      // The present value's current deviation from the computed average
    int ThreshDiff;     // abs(present value - custom Threshold);
    int derivative;     // current "instantaneous" derivative approximation using some set number of samples

    
    //Buffer Values


 void clearBuffer();
    void addValue(float);
    void fillValue(float, int);

    float getAverage();//get avg of the whole buffer

    float getElement(uint8_t idx);
    uint8_t getSize() { return _size; }
    uint8_t getCount() { return _cnt; }
int getBuffSum() {return _sum;}

    
 
    int bufferMin;        //value of the current lowest value in the buffer
    int bufferMax;
    int bufferAVG;        //Current avg of the whole buffer
    int index;          // location in array of newest value
    double buffertotal;           //the total value of the buffer at any given time
    
   
      //Inputs
    int getInput(int);
    void calibrateInput(int,unsigned int);
  
  void setupInput(int); // Pinnumber, 
  void setupInputandCal(int,int); // Pinnumber, calibration length
 // void setupInputwBuffer(int, int); // Pinnumber, sample size (for rolling buffers)


protected:
    uint8_t _size;
    uint8_t _cnt;
    uint8_t _idx;
    float   _sum;
    float * _ar;
  
   
	private:

    int _inputPin;
    
};






#endif
