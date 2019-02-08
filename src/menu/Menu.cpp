#include "Menu.h"

Menu::Menu( uint8_t num_cols, uint8_t num_rows, uint8_t lcd_addr )
{
    this->num_cols = num_cols;
    this->num_rows = num_rows;

    lcd = new Adafruit_LiquidCrystal( lcd_addr );
}

void Menu::setup()
{
    lcd->begin( num_cols, num_rows );
}