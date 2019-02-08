#include <Arduino.h>

#include "Encoder.h"

void Encoder::setup( uint8_t pinA, uint8_t pinB, void (*pinA_callback)(void), void (*pinB_callback)(void) )
{
    pinMode( pinA, INPUT_PULLUP );
    pinMode( pinB, INPUT_PULLUP );

    attachInterrupt( digitalPinToInterrupt( pinA ), pinA_callback, RISING );
    attachInterrupt( digitalPinToInterrupt( pinB ), pinB_callback, RISING );
}

uint8_t Encoder::getEncPos()
{
    return encoderPos;
}

void Encoder::pinA_callback()
{
    noInterrupts();

    // read all eight pin values then strip away all but pinA and pinB's values
    uint8_t reading = PIND & 0xC;

    //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    if( reading == B00001100 && aFlag )
    { 
        encoderPos--; //decrement the encoder's position count
        bFlag = 0; //reset flags for the next turn
        aFlag = 0; //reset flags for the next turn
    }
    else if( reading == B00000100 )
        //signal that we're expecting pinB to signal the transition to detent from free rotation
        bFlag = 1;

    interrupts();
}


void Encoder::pinB_callback()
{
    noInterrupts();

    //read all eight pin values then strip away all but pinA and pinB's values
    uint8_t reading = PIND & 0xC;

    //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    if( reading == B00001100 && bFlag )
    { 
        encoderPos++; //increment the encoder's position count
        bFlag = 0; //reset flags for the next turn
        aFlag = 0; //reset flags for the next turn
    }
    else if( reading == B00001000 ) 
        //signal that we're expecting pinA to signal the transition to detent from free rotation
        aFlag = 1;

    interrupts();
}