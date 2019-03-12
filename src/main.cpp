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
void bed_task();
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
float bed_2_temp = 0;
float bed_3_temp = 0;
float bed_4_temp = 0;
float bed_5_temp = 0;
float bed_6_temp = 0;
float bed_7_temp = 0;
float bed_8_temp = 0;
float bed_9_temp = 0;

bool global_on = false;
bool bed_1_on = false;
bool bed_2_on = false;
bool bed_3_on = false;
bool bed_4_on = false;
bool bed_5_on = false;
bool bed_6_on = false;
bool bed_7_on = false;
bool bed_8_on = false;
bool bed_9_on = false;

// Heated beds to control
Heater bed_1( HEATER_1_PIN, TEMP_1_PIN );
Heater bed_2( HEATER_2_PIN, TEMP_2_PIN );
Heater bed_3( HEATER_3_PIN, TEMP_3_PIN );
Heater bed_4( HEATER_4_PIN, TEMP_4_PIN );
Heater bed_5( HEATER_5_PIN, TEMP_5_PIN );
Heater bed_6( HEATER_6_PIN, TEMP_6_PIN );
Heater bed_7( HEATER_7_PIN, TEMP_7_PIN );
Heater bed_8( HEATER_8_PIN, TEMP_8_PIN );
Heater bed_9( HEATER_9_PIN, TEMP_9_PIN );

// Menu
MENU_INPUTS( in, &encStream );
MENU_OUTPUTS( out, MAX_DEPTH
    ,LCD_OUT( lcd, {0,0,20,4} )
    ,NONE
);

// Toggle menu items
TOGGLE( global_on, globalOnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_1_on, bed1OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_2_on, bed2OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_3_on, bed3OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_4_on, bed4OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_5_on, bed5OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_6_on, bed6OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_7_on, bed7OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_8_on, bed8OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( bed_9_on, bed9OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

// Bed Menus
MENU( allBedsMenu, "All Beds          >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( global_temp,"Global Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( globalOnToggle )
);

MENU( bed1Menu, "Bed 1             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_1_temp,"Bed 1 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed1OnToggle )
);

MENU( bed2Menu, "Bed 2             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_2_temp,"Bed 2 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed2OnToggle )
);

MENU( bed3Menu, "Bed 3             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_2_temp,"Bed 3 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed3OnToggle )
);

MENU( bed4Menu, "Bed 4             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_4_temp,"Bed 4 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed4OnToggle )
);

MENU( bed5Menu, "Bed 2             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_5_temp,"Bed 5 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed5OnToggle )
);

MENU( bed6Menu, "Bed 6             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_6_temp,"Bed 6 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed6OnToggle )
);

MENU( bed7Menu, "Bed 7             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_7_temp,"Bed 7 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed7OnToggle )
);

MENU( bed8Menu, "Bed 8             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_8_temp,"Bed 8 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed8OnToggle )
);

MENU( bed9Menu, "Bed 9             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_9_temp,"Bed 9 Temp: ","C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed9OnToggle )
);

// Main menu
MENU( mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    ,EXIT( "Info Screen       \01" )
    ,SUBMENU( allBedsMenu )
    ,SUBMENU( bed1Menu )
    ,SUBMENU( bed2Menu )
    ,SUBMENU( bed3Menu )
    ,SUBMENU( bed4Menu )
    ,SUBMENU( bed5Menu )
    ,SUBMENU( bed6Menu )
    ,SUBMENU( bed7Menu )
    ,SUBMENU( bed8Menu )
    ,SUBMENU( bed9Menu )
);

NAVROOT( nav, mainMenu, MAX_DEPTH, in,out );


// Scheduler / Tasks
Scheduler runner;
Task t1( 500, TASK_FOREVER, bed_task );
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
    lcd.createChar( 1, return_arrow );
    lcd.createChar( 2, degree );

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
void bed_task()
{
    bed_1.setTargetTemp( bed_1_temp );
    bed_2.setTargetTemp( bed_2_temp );
    bed_3.setTargetTemp( bed_3_temp );
    bed_4.setTargetTemp( bed_4_temp );
    bed_5.setTargetTemp( bed_5_temp );
    bed_6.setTargetTemp( bed_6_temp );
    bed_7.setTargetTemp( bed_7_temp );
    bed_8.setTargetTemp( bed_8_temp );
    bed_9.setTargetTemp( bed_9_temp );

    bed_1.enableHeater( bed_1_on );
    bed_2.enableHeater( bed_2_on );
    bed_3.enableHeater( bed_3_on );
    bed_4.enableHeater( bed_4_on );
    bed_5.enableHeater( bed_5_on );
    bed_6.enableHeater( bed_6_on );
    bed_7.enableHeater( bed_7_on );
    bed_8.enableHeater( bed_8_on );
    bed_9.enableHeater( bed_9_on );

    bed_1.update();
    bed_2.update();
    bed_3.update();
    bed_4.update();
    bed_5.update();
    bed_6.update();
    bed_7.update();
    bed_8.update();
    bed_9.update();
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
    static uint32_t old_time = 0;
    uint32_t cur_time = millis();

    if( cur_time - old_time > 500 )
    {
        char temp[5] = { '\0' };
        char trgt[5] = { '\0' };

        char screen[81] = {" "};
        screen[80] = '\0';

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_1.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_1.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 0 + 0], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_2.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_2.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 0 + 6], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_3.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_3.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 0 + 12], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_4.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_4.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 1 + 0], "%s/%s\2", temp, trgt );
        
        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_5.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_5.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 1 + 6], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_6.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_6.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 1 + 12], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_7.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_7.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 2 + 0], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_8.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_8.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 2 + 6], "%s/%s\2", temp, trgt );

        memset( temp, 0, sizeof(temp) );
        memset( trgt, 0, sizeof( trgt) );
        dtostrf( bed_9.getCurrTemp(), 2, 0, temp );
        dtostrf( bed_9.getTargetTemp(), 2, 0, trgt );
        sprintf( &screen[20 * 2 + 12], "%s/%s\2", temp, trgt );

        o.clear();
        o.setCursor( 0, 0 );
        o.print( screen );

        old_time = cur_time;
    }

    nav.idleChanged = true;
    return proceed;
}