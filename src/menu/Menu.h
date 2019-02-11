#ifndef MENU_H
#define MENU_H

#include <Adafruit_LiquidCrystal.h>


typedef struct
{
    char* text;
    
} menu_item_t;

// Forward declaration
struct _menu_t;

typedef struct _menu_t
{
    char* name;

    // The parent of this menu
    _menu_t*        parent;

    // The children of this menu
    _menu_t*        menu_entries;
    uint8_t         num_menu_entries;

    // The items to display in this menu
    menu_item_t*    item_entries;
    uint8_t         num_item_entries;
} menu_t;

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