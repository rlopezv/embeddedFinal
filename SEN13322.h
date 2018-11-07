#ifndef MBED_SEN1322_H
#define MBED_SEN1322_H

#include "mbed.h"

//Setup a new class for SEN1322 sensor
class SEN1322
{
public:
    SEN1322(PinName pin);
    operator float ();
    float read();
private:
//sets up the AnalogIn pin
    AnalogIn _pin;
}; 

#endif