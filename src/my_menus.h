#include <menu.h>


MENU(subMenu,"Bed 1",doNothing,anyEvent,noStyle
  ,OP("Sub1",doNothing,anyEvent)
  ,OP("Sub2",doNothing,anyEvent)
  ,OP("Sub3",doNothing,anyEvent)
  ,EXIT("<-Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(subMenu)
  ,EXIT("<-Back")
);
