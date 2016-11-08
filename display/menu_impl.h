/*
 * menu_impl.h
 *
 *  Created on: Nov 4, 2016
 *      Author: Valtteri
 */

#ifndef DISPLAY_MENU_IMPL_H_
#define DISPLAY_MENU_IMPL_H_

#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>

#include "menu.h"

#define MAX_OPTIONS 8

typedef Void(*DrawMenuFxn)(tContext*);
typedef void(*MenuOptionFxn)(void);

enum MenuId {
	MENU_MAIN = 0,
	MENU_ACTIVITIES,
	MENU_ACT_SUN,
	MENU_ACT_AIR,
	MENU_ACT_PHYS,
	MENU_HELP
};

typedef struct Menu* pMenu;

typedef struct {
	pMenu next;
	MenuOptionFxn action;
	const char *text;
} MenuOption;

typedef struct {
	const unsigned char id;
	const unsigned char numOptions;
	unsigned char selectedOption;
	DrawMenuFxn drawFxn;
	const MenuOption options[MAX_OPTIONS];
} Menu;

const Menu *GetFirstMenu();
int isLastOption(const MenuOption opt);


#endif /* DISPLAY_MENU_IMPL_H_ */
