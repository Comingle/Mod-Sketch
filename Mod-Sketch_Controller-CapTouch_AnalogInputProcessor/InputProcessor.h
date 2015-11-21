/*
  InputProcessor.h - Library for dealing with sensors in arduino in easier ways
  by Comingle / A Quitmeyer   Public Domain

  get calibration, buffer smoothing, derivatives, and scaling easily

  calibrating sensors hooked to analog inputs.
  InputProcessor is released under the
  hacked off examples by
  Copyright 2013 Julian Vidal
  https://github.com/poisa/Calibrator

  and Rob Tillaart
  arduino rolling average / smoothing example
*/

#ifndef InputProcessor_h
#define InputProcessor_h

#include "Arduino.h"

class InputProcessor
{
  public:


    //	InputProcessor(int thepin);
    InputProcessor(int buffersize, int thepin = -1);
    ~InputProcessor();
    unsigned int pin;               // onboard pin

    //InputProcessor funcs
    int update(); // just reads from analog pin
    int update(int funcval); // if you are not just using analogread, you can just pass an integer directly to it
    int update(int (*function)()); // if you want to just pass it a function from the main sketch that needs to continuously update

    void calibrate(int duration); // just reads from the analog pin that was setup
    void calibrate(int duration, int funcval);// if you are not just using analogread, you can just pass an integer directly to it
    void calibrate(int duration, int (*function)());// if you want to just pass it a function from the main sketch that needs to continuously update

    int rawValue;         //gets the most recent reading from the ADC, or value given
    int prevRawValue; //the value directly before
    int scaleValue(int currentValue);
    int scaleValue10bit(int currentValue);
    int scaledValue;      //gets the most recent reading scaled between the min and max, and mapped between 0 and 255

    void setValue(int currentValue);

    int getMin();
    int getMax();
    void resetCal();

    //Calibration Values
    bool calmode = false;
    int min = 724;        //the input's min from the calibration // default 0
    int max = 730;          //the input's max from the calibration  // default 1023


    int lastCal = 0;        //time in milliseconds of the last calibration (to find time since previous calibration do millis()-lastCal

    //Useful Values for Quick Access
    int offset = 100;        //A user defined offsetold or offset //  defaults to 127 if not specified
    int getAvgDiff() {
      return scaledValue - getAverage();
    }
    int getoffsetAvgDiff() {
      return getAverage() - offset;
    }
    //sometimes you just want the total difference that your value is away from a certain value
    int getabsoffsetAvgDiff() {
      return abs(getAverage() - offset);
    }

    int getoffsetDiff() {
      return scaledValue - offset;
    }
    int getabsoffsetDiff() {

      return abs(scaledValue - offset);
    }

    float getSTDEV();
    int getDerivative();
    int getscaledDerivative();

    //Buffer Values


    void clearBuffer();
    void addValue(float);
    void fillValue(float, int);

    float getAverage();//get avg of the whole buffer
    float getscaledAverage();
    float getElement(uint8_t idx);
    uint8_t getSize() {
      return _size;
    }
    uint8_t getCount() {
      return _cnt;
    }
    int getBuffSum() {
      return _sum;
    }




  protected:
    uint8_t _size;
    uint8_t _cnt;
    uint8_t _idx;
    float   _sum;
    float * _ar;


  private:


};






#endif
