/*
 * menu_impl.c
 *
 *  Created on: Nov 4, 2016
 *      Author: Valtteri
 */

#include "menu_impl.h"

Menu mainMenu;
Menu activitiesMenu;

Menu sunMenu = {
	.id = MENU_ACT_SUN,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawSunMenu,
	.options = {
			{&activitiesMenu, Sensors_StopTrackingSun, "Takaisin"},
			{NULL, NULL, ""}
	}
};

Menu activitiesMenu = {
	.id = MENU_ACTIVITIES,
	.numOptions = 5,
	.selectedOption = 0,
	.drawFxn = DrawActivitiesMenu,
	.options = {
			{&mainMenu, dummyFxn4, "Takaisin"},
			{&sunMenu, Sensors_StartTrackingSun, "Aurinko"},
			{NULL, dummyFxn4, "Raikas ilma"},
			{NULL, dummyFxn4, "Liikunta"},
			{NULL, dummyFxn1, "Kaverit"},
			{NULL, NULL, ""}
	}
};

Menu mainMenu = {
	.id = MENU_MAIN,
	.numOptions = 4,
	.selectedOption = 0,
	.drawFxn = DrawMainMenu,
	.options = {
			{NULL, dummyFxn1, "Luo uusi"},
			{NULL, dummyFxn2, "Hae/vie"},
			{&activitiesMenu, dummyFxn3, "Harrasta"},
			{NULL, powerButtonFxn, "Sammuta"},
			{NULL, NULL, ""}
	}
};

const Menu *GetFirstMenu()
{
	return &mainMenu;
}

int isLastOption(const MenuOption opt)
{
	return opt.action == NULL && opt.next == NULL;
}
