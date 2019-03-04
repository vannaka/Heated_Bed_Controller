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
#include "pin_defs.h"

using namespace Menu;


/******************************************************************************
 *                                 Defines
 *****************************************************************************/
// Encoder
#define encA 22
#define encB 24
#define encBtn 26

// Menu
#define MAX_DEPTH 2

/******************************************************************************
 *                          Function Declarations
 *****************************************************************************/
// Tasks
void bed_1_task();
void menu_task();

// Menu Callbacks
result displayInfo( menuOut& o,idleEvent e );

/******************************************************************************
 *                               Global Vars
 *****************************************************************************/
// Screen
LiquidCrystal_I2C lcd( 0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE );
uint8_t degree[8] = { 0xc,0x12,0x12,0xc,0x0,0x0,0x0,0x0 };
uint8_t return_arrow[8] = { 0x4,0xe,0x1f,0x4,0x1c,0x0,0x0,0x0 };

// Encoder
ClickEncoder clickEncoder( encA, encB, encBtn, 4 );
ClickEncoderStream encStream( clickEncoder, 1 );

// Heated bed control vars
float global_temp = 0;
float bed_1_temp = 0;
bool global_on = false;
bool bed_1_on = false;

// Heated beds to control
Heater bed_1( HEATER_1_PIN, TEMP_1_PIN );

// Menu
MENU_INPUTS( in, &encStream );
MENU_OUTPUTS( out, MAX_DEPTH
    ,LCD_OUT( lcd, {0,0,20,4} )
    ,NONE
);

TOGGLE( global_on, globalOnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_1_on, bed1OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

// All beds menu
MENU( allBedsMenu, "All Beds          >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( global_temp,"Global Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( globalOnToggle )
);

// Bed 1 menu
MENU( bed1Menu, "Bed 1             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_1_temp,"Bed 1 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed1OnToggle )
);

// Main menu
MENU( mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    ,EXIT( "Info Screen       \01" )
    ,SUBMENU( allBedsMenu )
    ,SUBMENU( bed1Menu )
);

NAVROOT( nav, mainMenu, MAX_DEPTH, in,out );


// Scheduler / Tasks
Scheduler runner;
Task t1( 500, TASK_FOREVER, bed_1_task );
Task t2( 100, TASK_FOREVER, menu_task );


/******************************************************************************
 *                                 Procedures
 *****************************************************************************/
// Encoder timer
void timerIsr() { clickEncoder.service(); }

void setup()
{
    Serial.begin( 9600 );

    // Setup menu
    nav.timeOut = 10;   // sec
    nav.showTitle = false;
    nav.idleTask = displayInfo;
    nav.exit();         // go to info screen

    // Setup lcd
    lcd.begin( 20,4 );
    lcd.createChar( 0, degree );
    lcd.createChar( 1, return_arrow );

    // Show splash screen
    lcd.setCursor( 0, 0 );
    lcd.print( "Not Your Mom's" );
    lcd.setCursor( 0, 1 );
    lcd.print( "Heated Bed" );
    lcd.setCursor( 0, 3 );
    lcd.print("By: Luke Mammen");
    delay( 5000 );

    // Timer for encoder knob
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
void bed_1_task()
{
    bed_1.setTargetTemp( bed_1_temp );
    bed_1.enableHeater( bed_1_on );
    bed_1.update();
}


void menu_task()
{
    nav.poll();
}


/**********************************************************
*   Info Screen
**********************************************************/
result displayInfo( menuOut& o, idleEvent e )
{
    // static uint32_t old_time = 0;
    // uint32_t cur_time = millis();

    // if( cur_time - old_time > 500 )
    {
        o.clear();
        o.setCursor( 0, 0 );
        o.print( bed_1.getCurrTemp(), 0 );
        o.print( "/" );
        o.print( bed_1.getTargetTemp(), 0 );
        o.write( uint8_t(0) );

        // old_time = cur_time;
    }

    nav.idleChanged = true;
    return proceed;
}