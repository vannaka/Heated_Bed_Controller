#ifndef MENU_H
#define MENU_H

#include <Adafruit_LiquidCrystal.h>


typedef struct
{
    const char* text;
    
} menu_item_t;

typedef struct menu_t
{
    const char* name;
    bool show_name_at_top;

    // The parent of this menu
    struct menu_t*        parent;

    // The children of this menu
    struct menu_t*  menu_entries;
    uint8_t         num_menu_entries;

    // The items to display in this menu
    menu_item_t*    item_entries;
    uint8_t         num_item_entries;
} menu_t;



class Menu
{
public:
    Menu( uint8_t num_cols, uint8_t num_rows, uint8_t lcd_addr );

    void setup( menu_t* topMenu );
    void update();

    void up();
    void down();
    void select();

private:
    Adafruit_LiquidCrystal* lcd;
    uint8_t num_cols;
    uint8_t num_rows;

    menu_t* menu_head;
    menu_t* curr_menu;

    uint8_t curr_line; // [0, num_rows)
    uint8_t curr_entry; // [0, curr_menu->num_menu_entries + curr_menu->num_item_entries)

};

#endif // MENU_H