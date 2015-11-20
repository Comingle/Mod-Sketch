



/***
*INPUT SECTION
*The part of the library dealing with accessing sensory inputs for the Device
***/

/*** SetupInput ***/

/*
//Setting up the input basically establishes a new clean buffer
//TODO FIX, NOTE RIGHT NOW THE SAMPLE SIZE IS NOT USED, and ALL BUFFERS ARE HARDCODED to 100
void OSSex::setupInput(int inNumber, int sampleSize) {


//Initialize the whole Array to zero
    for (int j=0; j<device._defaultInputBufferSize; j++) //This should be dynamic in the future
    {
    device.inputs[inNumber].buffer[j] = 0;

    }

  return;
}

//New version with Rolling input (will be folded into basic input in the future)
//Adapted from Arduino smoothing example (mellis igoe)
int OSSex::updateInput(int inNumber) {

int index = device.inputs[inNumber].index;                  // the index of the current reading
double total = device.inputs[inNumber].buffertotal;                  // the running total
int average = -1;
///Yeah this is the general idea, but of course this don't work, gotta play with memory shit andy
//Or vectors, but craig probably won't want the C++ library floating around here
//int readings[] = device.inputs[inNumber].buffer[] ;      //Copy the buffer over //Might be better to just have pointers
  

// subtract the last reading:
  total = total - device.inputs[inNumber].buffer[index];
//recheck if this value was the old min or max???

  
  // read from the sensor, and replace old index value with new:  
  device.inputs[inNumber].buffer[index] = analogRead(device.inputs[inNumber].pin); 
  // store this value as the current RAw value
   device.inputs[inNumber].rawValue = device.inputs[inNumber].buffer[index];
   //Store the scaled version of this value to the scaledValue var
    device.inputs[inNumber].scaledValue = map(device.inputs[inNumber].rawValue, 
    device.inputs[inNumber].min,device.inputs[inNumber].max,0,255);
  
  // add the reading to the total:
  total = total + device.inputs[inNumber].buffer[index];       
 device.inputs[inNumber].buffertotal =total;                  // the running total


  // calculate the average:
  average = total / device._defaultInputBufferSize;  
device.inputs[inNumber].bufferAVG=average;                // the average
//Calculate if we need to update the bufferMax and bufferMin

   if (device.inputs[inNumber].buffer[index] > device.inputs[inNumber].bufferMax) {
      device.inputs[inNumber].bufferMax = device.inputs[inNumber].buffer[index];
    }

    // record the minimum sensor value
    if (device.inputs[inNumber].buffer[index] < device.inputs[inNumber].bufferMin) {
      device.inputs[inNumber].bufferMin = device.inputs[inNumber].buffer[index];
    }
  
  
  
  // update the Structure
  

  
  //Add other handy values
device.inputs[inNumber].diffAVG = abs(average-device.inputs[inNumber].buffer[index]);
device.inputs[inNumber].diffThresh = abs( device.inputs[inNumber].customThreshold-device.inputs[inNumber].buffer[index] );

//Calculate the derivative with an episilon the size of one sample
int delta = 0;
int one = device.inputs[inNumber].buffer[index];
int oneminust= 0;
if(index==0){
oneminust = device.inputs[inNumber].buffer[device._defaultInputBufferSize];

}
else{
oneminust = device.inputs[inNumber].buffer[index-1];
}

delta = one-oneminust;
device.inputs[inNumber].derivative = delta/1; // just currently dividing by the sample size;
  
  // advance to the next position in the array for the next time:  
  index = index + 1;                    
  // if we're at the end of the array...
  if (index >= device._defaultInputBufferSize)  {            
    // ...wrap around to the beginning: 
    index = 0;                           
}
  device.inputs[inNumber].index=index;                  // the index of the current reading

  return analogRead(device.inPins[inNumber]);
}

/*** Calibration ***/
//Adapted from Arduino example (Mellis, Igoe)
//specify sensor number and duration of calibration
//
//void calibrateInput(int inNumber, unsigned int duration) {
//const int sensorPin = device.inputs[inNumber].pin;    // pin that the sensor is attached to
//
//  int sensorValue = 0;         // the sensor value
//int sensorMin = 2200;        // minimum sensor value
//int sensorMax = -1;           // maximum sensor value
//double total = 0;
//double loopcounter=0;
//  
//  // calibrate during n milliseconds 
//  while (millis() < duration) {
//    sensorValue = analogRead(sensorPin);
//
//  
//     // record the maximum sensor value
//    if (sensorValue > sensorMax) {
//      sensorMax = sensorValue;
//    }
//
//    // record the minimum sensor value
//    if (sensorValue < sensorMin) {
//      sensorMin = sensorValue;
//    }
//  
//
//  total=total+sensorValue;
//  loopcounter++;
//  }
//  
//  //asssign values back to the STRUCT
//  
//  device.inputs[inNumber].min = sensorMin;
//  device.inputs[inNumber].max = sensorMax;
//  device.inputs[inNumber].avg = 1.0*total/loopcounter;
//  
//  
//  
//  //Just for the hell of it, return the currently read value mapped to the calibration
//  /*
//     sensorValue = analogRead(sensorPin);
//   sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);
//    // in case the sensor value is outside the range seen during calibration
//  sensorValue = constrain(sensorValue, 0, 255);
//  */
//  
//  //Record a timestamp for when this calibration took place
//    device.inputs[inNumber].lastCal = millis();
//  return;
//}

/**/
