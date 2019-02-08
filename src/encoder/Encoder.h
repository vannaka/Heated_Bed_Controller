#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

class Encoder
{
public:
    // Encoder( uint8_t pinA, uint8_t pinB );

    void setup( uint8_t pinA, uint8_t pinB, void (*pinA_callback)(void), void (*pinB_callback)(void));
    uint8_t getEncPos();

    void pinA_callback();
    void pinB_callback();

private:
    uint8_t pinA;
    uint8_t pinB;

    volatile uint8_t aFlag = 0;
    volatile uint8_t bFlag = 0; 
    volatile uint8_t encoderPos = 0;
    volatile uint8_t oldEncPos = 0;

};

#endif //ENCODER_H