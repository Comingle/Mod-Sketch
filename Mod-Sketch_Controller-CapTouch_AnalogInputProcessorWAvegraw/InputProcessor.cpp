/*
 * 
 * 
 * Hacked upversion of Julian Vidal 's    https://github.com/poisa/Calibrator
  Calibrator.cpp - Library for calibrating sensors hooked to analog inputs.
  Calibrator is released under the Apache 2.0 license. See the included LICENSE file.
  Copyright 2013 Julian Vidal

*/

#include "Arduino.h"
#include "InputProcessor.h"
#include <stdlib.h>

InputProcessor::InputProcessor(int n)
{

    _size = n;
    _ar = (float*) malloc(_size * sizeof(float));
    if (_ar == NULL) _size = 0;
    clearBuffer();
    
  //_inputPin=thepin;

 pin = -1;  
}


InputProcessor::~InputProcessor()
{
    if (_ar != NULL) free(_ar);
}

void InputProcessor::setupInput(int thepin){
pin=thepin;

}

/*
 * Takes input if already calibrated to just use a sensor pin
 */
int InputProcessor::update()
{
rawValue = analogRead(pin);



addValue(rawValue); //add the value to our buffer
scaledValue= scaleValue(rawValue);
return scaledValue;
}

/*
 * Takes input from the output of a different sensor function
 */
int InputProcessor::update(int funcval){
rawValue=funcval;
addValue(rawValue); //add the value to our buffer
scaledValue= scaleValue(rawValue);



return scaledValue;

}
//Takes in a value from an arbitrary function you setup and feed to it
int InputProcessor::update(int (*callback)()){
rawValue=(int)callback();

addValue(rawValue); //add the value to our buffer

scaledValue= scaleValue(rawValue);



return scaledValue;

}


//calibrate for a certain duration in millis
//Used on basic analog input where we already setup the pin
void InputProcessor::calibrate(int duration)
{
  long ctime= millis();
    resetCal();
while (millis()-ctime < duration) {
  Serial.println(millis()-ctime);
  
        setValue(analogRead(pin));
    }
    lastCal = millis();
    calmode=false;
}

//calibrate for a certain duration in millis
//Used on input with some outside function providing the value
void InputProcessor::calibrate(int duration, int funcval)
{
long ctime= millis();
    resetCal();
while (millis() < duration+ctime) {
  Serial.println(millis()-ctime);
        setValue(funcval);
    }
    lastCal = millis();
    calmode=false;
}

//Takes in a value from an arbitrary function you setup and feed to it
void InputProcessor::calibrate(int duration, int (*callback)()){
long ctime= millis();
    resetCal();
while (millis() < duration+ctime) {
  Serial.println(millis()-ctime);
        setValue((int)callback());
    }
    lastCal = millis();
    calmode=false;
}

 

void InputProcessor::setValue(int currentValue)
{
	if (currentValue > max) {
		max = currentValue;
	}

	if (currentValue < min) {
		min = currentValue;
	}
};

/*
 *Takes in a value and scales it between 0-255 for use with motors and leds and other PWM
 */

int InputProcessor::scaleValue(int currentValue)
{
	currentValue = map(currentValue, min, max, 0, 255);
	currentValue = constrain(currentValue, 0, 255);
	return currentValue;
}


/*
 *Takes in a value and scales it between 0-1023 for full resolution use
 */
int InputProcessor::scaleValue10bit(int currentValue)
{
  currentValue = map(currentValue, min, max, 0, 1023);
  currentValue = constrain(currentValue, 0, 1023);
  return currentValue;
}

int InputProcessor::getMin() 
{
	return min;
}

int InputProcessor::getMax() 
{
	return max;
}

void InputProcessor::resetCal()
{
	min = 32000;
	max = -32000;
}






// resets all counters
void InputProcessor::clearBuffer()
{
    _cnt = 0;
    _idx = 0;
    _sum = 0.0;
    for (int i = 0; i< _size; i++) _ar[i] = 0.0;  // needed to keep addValue simple
}

// adds a new value to the data-set
void InputProcessor::addValue(float f)
{
    if (_ar == NULL) return;
    _sum -= _ar[_idx];
    _ar[_idx] = f;
    _sum += _ar[_idx];
    _idx++;
    if (_idx == _size) _idx = 0;  // faster than %
    if (_cnt < _size) _cnt++;
}


// returns the average of the data-set added sofar
float InputProcessor::getAverage()
{
    if (_cnt == 0) return NAN;
    return _sum / _cnt;
}

// returns the average of the data-set added sofar scaled between max and min
float InputProcessor::getscaledAverage()
{
    if (_cnt == 0) return NAN;
    return scaleValue(_sum / _cnt);
}


// returns the value of an element if exist, 0 otherwise
float InputProcessor::getElement(uint8_t idx)
{
    if (idx >=_cnt ) return NAN;
    return _ar[idx];
}

// fill the average with a value
// the param number determines how often value is added (weight)
// number should preferably be between 1 and size
void InputProcessor::fillValue(float value, int number)
{
    clearBuffer();
    for (int i = 0; i < number; i++)
    {
        addValue(value);
    }
}

