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
			{&activitiesMenu, Menu_StopS, "Takaisin"},
			{NULL, NULL, ""}
	}
};

Menu airMenu = {
	.id = MENU_ACT_AIR,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawAirMenu,
	.options = {
			{&activitiesMenu, Menu_StopA, "Takaisin"},
			{NULL, NULL, ""}
	}
};

Menu physMenu = {
	.id = MENU_ACT_PHYS,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawPhysMenu,
	.options = {
			{&activitiesMenu, Menu_StopP, "Takaisin"},
			{NULL, NULL, ""}
	}
};

Menu socialMenu = {
	.id = MENU_ACT_SOCIAL,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawSocialMenu,
	.options = {
			{&activitiesMenu, NULL, "Takaisin"},
			{NULL, NULL, ""}
	}
};

Menu activitiesMenu = {
	.id = MENU_ACTIVITIES,
	.numOptions = 5,
	.selectedOption = 0,
	.drawFxn = DrawActivitiesMenu,
	.options = {
			{&mainMenu, NULL, "Takaisin"},
			{&sunMenu, Menu_StartS, "Aurinko"},
			{&airMenu, Menu_StartA, "Raikas ilma"},
			{&physMenu, Menu_StartP, "Liikunta"},
			{&socialMenu, NULL, "Kaverit"},
			{NULL, NULL, ""}
	}
};

Menu mainMenu = {
	.id = MENU_MAIN,
	.numOptions = 4,
	.selectedOption = 0,
	.drawFxn = DrawMainMenu,
	.options = {
			{NULL, NULL, "Luo uusi"},
			{NULL, NULL, "Hae/vie"},
			{&activitiesMenu, NULL, "Harrasta"},
			{NULL, NULL, "Sammuta"},
			{NULL, NULL, ""}
	}
};

const Menu *GetFirstMenu()
{
	return &mainMenu;
}

int isLastOption(const MenuOption opt)
{
	return opt.action == NULL && opt.next == NULL && !strlen(opt.text);
}
