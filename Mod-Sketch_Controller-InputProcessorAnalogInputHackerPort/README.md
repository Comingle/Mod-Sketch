# Calibrator
### Arduino library for calibrating sensors hooked to analog inputs

Version 1.0 Created by [Julian Vidal](http://julianvidal.com/)

## Why use this library

I know there are dozens of ways to calibrate sensors but I wasn't too happy with the ones I found online so I decided to write my own. I'm not claiming that this is the fastest calibration method nor the best; it's just my take on the subject.

The philosophy behind this library is that you never work with the sensor values directly but rather with the values provided by the library. If the sensors have gone through calibration (which this library provides) then you can always assume that the values you get range from 0 to 1023. 

## Why you need to calibrate your analog input sensors

Arduino will always return a number from 0 to 1023 when you call analogRead() on a pin. The problem is that your sensor might give you different minimum and maximum numbers inside that range.  For example, a photoresistor may give you a minimum value of 30 and a maximum value of 600 depending on the ambient light available at the time you do the measuring. So if you hardcode this numbers in your Arduino code and then turn on more lights or take your project someplace where the lighting conditions are different, you will get unexpected results because your sensors will be calibrated to some other environment. 

When you ask the library for a value -after it goes through its calibration phase- it will always interpret the lowest sensor value as 0 and the highest sensor value as 1023 no matter what these values actually are. So when you program your Arduino code, you can hardcode numbers and these numbers will be always meaningful as long as the sensors are calibrated. For our example, 1023 will always mean "the highest value my photoresistor will return in this lighting conditions".   

## Installation

Download and unzip the library into your Arduino library directory. Restart the Arduino IDE. If you did everything right, you should be able to find the library under Sketch > Import LIbrary... > Calibrator.

## Usage

Here's a simple script that uses one Calibrator object. If you need to calibrate more sensors you can always use a Calibrator array; you need one calibrator object per sensor. You CANNOT use one Calibrator object with more than one sensor.

```C++
#include <Calibrator.h>

Calibrator calibrator;

int sensorPin = A0; // The sensor we want to calibrate
int ledPin = 13;    // Will indicate when calibration is going on

void setup()
{
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH); // Turn LED on
    Serial.begin(9600);
    
    // Reset the calibrator. You only need to call reset() if you restart calibration again but
    // if calibration is only run once like in this example is not needed. It doesn't hurt to 
    // call it here though. 
    calibrator.reset();
    
    // Run calibration automatically during the first 
    // five seconds (or 5000 milliseconds) your code runs.
    // It is important that during calibration you "exercise" your sensors to measure both 
    // ends of their range.
    while (millis() < 5000) {
        calibrator.setValue(analogRead(sensorPin));
    }
    digitalWrite(ledPin, LOW); // Turn LED off
}

void loop()
{
    // In your code analogRead(sensorPin) will give you the uncalibrated value
    // and calibrator.getValue(analogRead(sensorPin)) will give you the calibrated value 
    
    Serial.print("Sensor value: ");
    Serial.print(analogRead(sensorPin));
    Serial.print(" , Calibrator value: ");
    Serial.println(calibrator.getValue(analogRead(sensorPin)));
}
```

## PERFORMANCE

I've been using this library for a while before publishing it and never had any performance problems with it. Having said that, each call to getValue() has an internal call to map() and constrain(). You should do some performance testing to see if this will affect you in some way.

## License

Calibrator is released under the Apache 2.0 license. See the included LICENSE file.
 
## VERSION HISTORY

2013-07-08 First version