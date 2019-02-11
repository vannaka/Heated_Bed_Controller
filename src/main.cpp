#include <Arduino.h>
#include <TaskScheduler.h>
#include <Adafruit_LiquidCrystal.h>

#include "encoder/Encoder.h"

#include "heater/heater.h"
#include "heater/pin_defs.h"

#include "menu/Menu.h"
#include "my_menus.h"


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

Menu menu( 16, 2, 0 );

Scheduler runner;
Task t1( 100, TASK_FOREVER, bed_0_task );
Task t2( 500, TASK_FOREVER, lcd_task );

void setup()
{
    Serial.begin( 9600 );

    // Setup encoder knob
    encoder.setup( 2, 3, []{encoder.pinA_callback();}, []{encoder.pinB_callback();} );

    // Setup menu
    menu.setup( &main_menu );

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


/**********************************************************
*   Tasks
**********************************************************/
void bed_0_task()
{
    bed_0.update();
}


void lcd_task()
{
    menu.update();
    // // currTemp*/setpoint*
    // lcd.setCursor( 0, 0 );
    // lcd.print( bed_0.getCurrTemp(), 1 );
    // lcd.write( 3 );
    // lcd.print("/");
    // lcd.print( bed_0.getTargetTemp(), 1 );
    // lcd.write( 3 );
}
