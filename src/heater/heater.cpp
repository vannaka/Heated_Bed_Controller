#include <Arduino.h>
#include "heater.h"
#include "temp_table.h"

#define COUNT(a) ( sizeof(a) / sizeof(*a) )

#define PID_CONTROL_RANGE 20
#define Heater_MAX_POWER 150
#define DEAD_TIME 80 // in increments of 100ms
#define NUM_SAMPLES 16

Heater::Heater( uint8_t heaterPin, uint8_t tempPin )
{
    this->heaterPin = heaterPin;
    this->tempPin = tempPin;

    pinMode( heaterPin, OUTPUT );
    pinMode( tempPin, INPUT );

    currTemp = 20.0; // Room temp
    deltaTemp = 0.0;
    enabled = false;
    heaterPower = 0;
    IStateLimitMin = 0;
    IStateLimitMax = Heater_MAX_POWER;
}

void Heater::update()
{
    getTemp();

    if( enabled )
    {
        calcPower();
        setHeaterPower( heaterPower );
    }
    else
    {
        setHeaterPower( 0 );
    }
}

void Heater::setTargetTemp( float temp )
{
    targetTemp = temp;
}

float Heater::getTargetTemp()
{
    return targetTemp;
}

void Heater::getTemp()
{
    // Get raw value from thermistor
    int raw = 0;

    for( int i = 0; i < NUM_SAMPLES; i++ )
    {
        raw += analogRead( tempPin );
    }

    raw = raw >> 2;

    size_t i = 0;
    float celsius = 0;
    
    // Lookup temperature in table
    for( i = 1; i < COUNT( temp_table ); i++ )
    {
        if( temp_table[i][0] > raw )
        {
            celsius = temp_table[i - 1][1] +
                        ( raw - temp_table[i - 1][0] ) *
                        (float)( temp_table[i][1] - temp_table[i - 1][1] ) /
                        (float)( temp_table[i][0] - temp_table[i - 1][0] );
            break;
        }
    }

    // Overflow: Set to last value in the table
    if( i == COUNT( temp_table ) )
    {
        celsius = temp_table[i - 1][1];
    }

    // Update memeber variables
    prevTemp = currTemp;
    currTemp = celsius;

    prevDeltaTemp = deltaTemp;
    deltaTemp = currTemp - prevTemp;
}

int Heater::getPower()
{
    return heaterPower;
}

void Heater::setHeaterPower( int power )
{
    analogWrite( heaterPin, power );
}

void Heater::enableHeater( bool enable )
{
    this->enabled = enable;
    // enableTime = millis();
}

float Heater::getCurrTemp()
{
    return currTemp;
}

void Heater::calcPower()
{
    // Local variables
    float error;
    float predictedTemp;

    // Initialize local variables
    error = targetTemp - currTemp;
    predictedTemp = 0.0;

    // Heating off
    if( targetTemp < 20.0 )
    {
        heaterPower = 0;
    }
    // Phase 1: full heating until control range reached
    else if( error > PID_CONTROL_RANGE )
    {
        heaterPower = Heater_MAX_POWER;

        IStateLimitMax = Heater_MAX_POWER;
        IStateLimitMin = 0;
    }
    // Bed is significantly hotter than target temp
    else if( error < -PID_CONTROL_RANGE )
    {
        heaterPower = 0;
    }
    // control range handle by heat manager
    else
    {
        // Peak reached
        if( prevDeltaTemp >= 0 && deltaTemp < 0 )
        {
            if( error < -0.5 )
            {
                IStateLimitMax = constrain( IStateLimitMax - 10, IStateLimitMin, Heater_MAX_POWER );
            }
            else
            {
                IStateLimitMax = constrain( IStateLimitMax + 10, IStateLimitMin, Heater_MAX_POWER );
            }
        }
        // Bottom reached
        else if( prevDeltaTemp <= 0 && deltaTemp > 0 )
        {
            if( error > 0.5 )
            {
                IStateLimitMin = constrain( IStateLimitMin + 10, 0, IStateLimitMax - 20 );
            }
            else
            {
                IStateLimitMin = constrain( IStateLimitMin - 10, 0, IStateLimitMax - 20 );
            }
        }

        // Calculate the temp after the dead time
        predictedTemp = currTemp + deltaTemp * DEAD_TIME;

        heaterPower = predictedTemp > targetTemp ? IStateLimitMin : IStateLimitMax;
    }
}