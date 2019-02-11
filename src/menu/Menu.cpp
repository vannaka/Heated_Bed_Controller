#include "Menu.h"


// Custome characters
uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4 };
uint8_t penis[8]    = { 0x4,0xa,0xe,0xa,0xa,0x11,0x15,0xa };
uint8_t boob_half[8] = { 0x0,0xe,0x11,0x15,0x11,0xe,0x0,0x0 };
uint8_t degree[8] = { 0xc,0x12,0x12,0xc,0x0,0x0,0x0,0x0 };


Menu::Menu( uint8_t num_cols, uint8_t num_rows, uint8_t lcd_addr )
{
    this->num_cols = num_cols;
    this->num_rows = num_rows;

    lcd = new Adafruit_LiquidCrystal( lcd_addr );
}


void Menu::setup( menu_t* topMenu )
{
    menu_head = topMenu;
    curr_menu = topMenu;

    lcd->begin( num_cols, num_rows );
    lcd->setBacklight( HIGH );
    lcd->home();
    lcd->createChar( 0, retarrow );
    lcd->createChar( 1, penis );
    lcd->createChar( 2, boob_half );
    lcd->createChar( 3, degree );
}


void Menu::up()
{
    
}


void Menu::down()
{

}


void Menu::select()
{

}