#include <Arduino.h>
#include <TaskScheduler.h>
#include <Adafruit_LiquidCrystal.h>

#include "encoder/Encoder.h"

#include "heater/heater.h"
#include "heater/pin_defs.h"

// LCD
// Custome characters
uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4 };
uint8_t penis[8]    = { 0x4,0xa,0xe,0xa,0xa,0x11,0x15,0xa };
uint8_t boob_half[8] = { 0x0,0xe,0x11,0x15,0x11,0xe,0x0,0x0 };
uint8_t degree[8] = { 0xc,0x12,0x12,0xc,0x0,0x0,0x0,0x0 };

Adafruit_LiquidCrystal lcd( 0 );

// Encoder
uint8_t encoderPos = 0; 
uint8_t oldEncPos = 0;

Encoder encoder;

// Callback functions
void bed_0_task();
void lcd_task();

// Heated beds to control
Heater bed_0( HEATER_0_PIN, TEMP_0_PIN );

Scheduler runner;
Task t1( 100, TASK_FOREVER, bed_0_task );
Task t2( 500, TASK_FOREVER, lcd_task );

void setup()
{
    Serial.begin( 9600 );

    // Setup encoder knob
    encoder.setup( 2, 3, []{encoder.pinA_callback();}, []{encoder.pinB_callback();} );

    // Setup LCD
    lcd.begin( 16, 2 );
    lcd.setBacklight( HIGH );
    lcd.createChar( 0, retarrow );
    lcd.createChar( 1, penis );
    lcd.createChar( 2, boob_half );
    lcd.createChar( 3, degree );
    lcd.home();

    // Setup heated beds
    bed_0.setTargetTemp( 40.0 );
    bed_0.enableHeater( true );

    // Setup cooperative scheduler
    runner.init();
    runner.addTask( t1 );
    runner.addTask( t2 );

    t1.enable();
    t2.enable();
}

void loop() 
{
    runner.execute();
}

void updateLCD()
{
    // currTemp*/setpoint*
    lcd.setCursor( 0, 0 );
    lcd.print( bed_0.getCurrTemp(), 1 );
    lcd.write( 3 );
    lcd.print("/");
    lcd.print( bed_0.getTargetTemp(), 1 );
    lcd.write( 3 );
}

/**********************************************************
*   Tasks
**********************************************************/
void bed_0_task()
{
    bed_0.update();
}

void lcd_task()
{
    updateLCD();
}
