#include <Arduino.h>
#include <math.h>

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

// Beds
#define NUM_BEDS 9

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
float bed_temps[NUM_BEDS] = { 0 };

bool global_on = false;
bool beds_on[NUM_BEDS] = { false };

// Heated beds to control
Heater beds[NUM_BEDS] = 
{
    Heater( HEATER_1_PIN, TEMP_1_PIN ),
    Heater( HEATER_2_PIN, TEMP_2_PIN ),
    Heater( HEATER_3_PIN, TEMP_3_PIN ),
    Heater( HEATER_4_PIN, TEMP_4_PIN ),
    Heater( HEATER_5_PIN, TEMP_5_PIN ),
    Heater( HEATER_6_PIN, TEMP_6_PIN ),
    Heater( HEATER_7_PIN, TEMP_7_PIN ),
    Heater( HEATER_8_PIN, TEMP_8_PIN ),
    Heater( HEATER_9_PIN, TEMP_9_PIN ),
};

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

TOGGLE( beds_on[0], bed1OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[1], bed2OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[2], bed3OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[3], bed4OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[4], bed5OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[5], bed6OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[6], bed7OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[7], bed8OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
  ,VALUE( "On", true, doNothing, noEvent )
  ,VALUE( "Off", false, doNothing, noEvent )
);

TOGGLE( beds_on[8], bed9OnToggle, "Enable: ", doNothing, noEvent, wrapStyle
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
    ,FIELD( bed_temps[0], "Bed 1 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed1OnToggle )
);

MENU( bed2Menu, "Bed 2             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[1], "Bed 2 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed2OnToggle )
);

MENU( bed3Menu, "Bed 3             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[2], "Bed 3 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed3OnToggle )
);

MENU( bed4Menu, "Bed 4             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[3], "Bed 4 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed4OnToggle )
);

MENU( bed5Menu, "Bed 2             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[4], "Bed 5 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed5OnToggle )
);

MENU( bed6Menu, "Bed 6             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[5], "Bed 6 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed6OnToggle )
);

MENU( bed7Menu, "Bed 7             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[6], "Bed 7 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed7OnToggle )
);

MENU( bed8Menu, "Bed 8             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[7], "Bed 8 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
    ,SUBMENU( bed8OnToggle )
);

MENU( bed9Menu, "Bed 9             >", doNothing, anyEvent, noStyle
    ,EXIT( "Main              \01" )
    ,FIELD( bed_temps[8], "Bed 9 Temp: ", "C", 0, 60, 5, 1, doNothing, noEvent, noStyle )
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
    for( int i = 0; i < NUM_BEDS; i++ )
    {
        beds[i].setTargetTemp( bed_temps[i] );
        beds[i].enableHeater( beds_on[i] );
        beds[i].update();
    }
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
    static uint32_t old_time = millis();
    uint32_t cur_time = millis();

    if( cur_time - old_time > 500 )
    {
        char screen[81] = {" "};
        screen[80] = '\0';

        for( int i = 0; i < NUM_BEDS; i++ )
        {
            char* entry = screen + ( 20 * ( i / 3 ) ) + ( ( i % 3 ) * 6 );

            float curr_tmp = beds[i].getCurrTemp();
            float trgt_tmp = beds[i].getTargetTemp();

            // Only display target temp if heater is on.
            if( !beds_on[i] )
                trgt_tmp = 0.0;

            // NOTE: the arduino does not support %f modifiers in any
            //  printf varient. We round as a workarount.
            sprintf( entry , "%ld/%ld", round( curr_tmp ), round( trgt_tmp ) );
        }

        // Get rid of null terminators added by sprintf
        for( int i = 0; i < 80; i ++ )
        {
            if( screen[i] == '\0' )
                screen[i] = ' ';
        }

        // For some reason this swaps lines 2 and 3;
        // o.setCursor( 0, 0 );
        // o.print( screen );

        // For whatever reason, the bellow prints the lines
        //  in the correct order while the above swaps 
        //  lines 2 and 3.
        o.setCursor( 0, 0 );
        o.write( screen, 20 );
        o.setCursor( 0, 1 );
        o.write( screen + 20, 20 );
        o.setCursor( 0, 2 );
        o.write( screen + 40, 20 );
        o.setCursor( 0, 3 );
        o.write( screen + 60, 20 );

        old_time += 500;
    }

    nav.idleChanged = true;
    return proceed;
}