#include <common/menu.h>

int main() {
	//create the menu-object
	MenuStruct* menu = MenuStruct::acquire(true, menuGlobalLoad);
	
	//run the menu
	menu->run(menuroot::menuID);

	//release the menu and return
	menu->release();
	return 0;
}