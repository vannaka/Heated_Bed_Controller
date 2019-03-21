#ifndef HEATER_H
#define HEATER_H

#include <stdint.h>

class Heater
{

public:
    Heater( uint8_t heaterPin, uint8_t tempPin );

    void update();
    void setTargetTemp( float temp );
    float getTargetTemp();
    void enableHeater( bool enable );
    float getCurrTemp();
    int getPower();

private:
    uint8_t heaterPin;
    uint8_t tempPin;

    bool enabled;
    // unsigned long enableTime;   // This will overflow after 50 days. Maybe a problem?

    float targetTemp;
    float currTemp;

    float deltaTemp;
    float prevDeltaTemp;

    int heaterPower;

    int IStateLimitMin;
    int IStateLimitMax;

    void getTemp();
    void setHeaterPower( int power );

    void calcPower();

};

#endif