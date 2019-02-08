#ifndef MENU_H
#define MENU_H

#include <Adafruit_LiquidCrystal.h>

class Menu
{
public:
    Menu( uint8_t num_cols, uint8_t num_rows, uint8_t lcd_addr );

    void setup();

private:
    Adafruit_LiquidCrystal* lcd;
    uint8_t num_cols;
    uint8_t num_rows;

};

#endif // MENU_H