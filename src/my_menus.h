#include "menu/Menu.h"

menu_t main_menu;

menu_t bed_menu_0 = 
{
    "Bed 0",
    true,
    &main_menu,
    NULL,
    0,
    (menu_item_t[])
    {
        {
            "Setpoint: "
        }
    },
    1
};

menu_t main_menu = 
{
    "Main Menu",
    true,
    NULL,
    (menu_t[]){ bed_menu_0 },
    1,
};