#include <Arduino.h>
#include <TaskScheduler.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <menu.h>
#include <menuIO/lcdOut.h>
#include <TimerOne.h>
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

#include "heater/heater.h"
#include "heater/pin_defs.h"

using namespace Menu;


/******************************************************************************
 *                                 Defines
 *****************************************************************************/
// Encoder
#define encA 24
#define encB 22
#define encBtn 26

// Menu
#define MAX_DEPTH 2

/******************************************************************************
 *                          Function Declarations
 *****************************************************************************/
// Tasks
void bed_0_task();
void menu_task();

// Menu Callbacks

/******************************************************************************
 *                               Global Vars
 *****************************************************************************/
// Screen
LiquidCrystal_I2C lcd( 0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE );

// Encoder
ClickEncoder clickEncoder( encA, encB, encBtn, 4 );
ClickEncoderStream encStream( clickEncoder, 1 );

// Heated bed control vars
int8_t global_temp = 0;
bool global_on = false;

// Menu
MENU_INPUTS( in, &encStream );
MENU_OUTPUTS( out, MAX_DEPTH
    ,LCD_OUT( lcd, {0,0,20,4} )
    ,NONE
);

TOGGLE( global_on, globalOnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "True", true, doNothing, noEvent )
  ,VALUE( "False", false, doNothing, noEvent )
);

// All beds menu
MENU( allBedsMenu, "All Beds", doNothing, anyEvent, noStyle
    ,FIELD( global_temp,"Glocal Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( globalOnToggle )
    ,EXIT( "<-Back" )
);

// Bed 1 menu
MENU( subMenu, "Bed 1", doNothing, anyEvent, noStyle
    ,OP( "Sub1", doNothing,anyEvent )
    ,OP( "Sub2", doNothing,anyEvent )
    ,OP( "Sub3", doNothing,anyEvent )
    ,EXIT( "<-Back" )
);

// Main menu
MENU( mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    ,SUBMENU( allBedsMenu )
    ,SUBMENU( subMenu )
    ,EXIT( "<-Back" )
);

NAVROOT( nav, mainMenu, MAX_DEPTH, in,out );

// Heated beds to control
Heater bed_0( HEATER_0_PIN, TEMP_0_PIN );

// Scheduler / Tasks
Scheduler runner;
Task t1( 100, TASK_FOREVER, bed_0_task );
Task t2( 100, TASK_FOREVER, menu_task );


/******************************************************************************
 *                                 Procedures
 *****************************************************************************/
// Encoder timer
void timerIsr() { clickEncoder.service(); }

void setup()
{
    Serial.begin( 9600 );
    while(!Serial);

    // Setup menu
    nav.showTitle = true;
    lcd.begin( 20,4 );
    lcd.setCursor( 0, 0 );
    lcd.print( "Not Your Mom's" );
    lcd.setCursor( 0, 1 );
    lcd.print( "Heated Bed" );
    lcd.setCursor( 0, 3 );
    lcd.print("By: Luke Mammen");
    delay( 5000 );

    Timer1.initialize( 1000 );
    Timer1.attachInterrupt( timerIsr );

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


void menu_task()
{
    nav.poll();
}
