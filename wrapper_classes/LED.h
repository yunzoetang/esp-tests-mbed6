#include "mbed.h"

#ifndef LED_H
#define LED_H
class LED                                           //Begin LED class definition
{
protected:                                          //Protected (Private) data member declaration
    DigitalOut outputSignal;                        //Declaration of DigitalOut object
    bool status;                                    //Variable to recall the state of the LED

public:                                             //Public declarations
    LED(PinName pin) : outputSignal(pin){off();}    //Constructor - user provided pin name is assigned to the DigitalOut

    void on(void)                                   //Public member function for turning the LED on
    {
        outputSignal = 0;                           //Set output to 0 (LED is active low)
        status = true;                              //Set the status variable to show the LED is on
    }

    void off(void)                                  //Public member function for turning the LED off
    {
        outputSignal = 1;                           //Set output to 1 (LED is active low)
        status = false;                             //Set the status variable to show the LED is off
    }

    void toggle(void)                               //Public member function for toggling the LED
    {
        if (status)                                 //Check if the LED is currently on
            off();                                  //Turn off if so
        else                                        //Otherwise...
            on();                                   //Turn the LED on
    }

    bool getStatus(void)                            //Public member function for returning the status of the LED
    {
        return status;                              //Returns whether the LED is currently on or off
    }
};

#endif