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
			{&activitiesMenu, {Menu_StopRedrawing, Sensors_StopTrackingSun, NULL}, "Takaisin"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

Menu airMenu = {
	.id = MENU_ACT_AIR,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawAirMenu,
	.options = {
			{&activitiesMenu, {Menu_StopRedrawing, Sensors_StopTrackingFreshAir, NULL}, "Takaisin"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

Menu physMenu = {
	.id = MENU_ACT_PHYS,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawPhysMenu,
	.options = {
			{&activitiesMenu, {Menu_StopRedrawing, Sensors_StopTrackingPhysical, NULL}, "Takaisin"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

Menu socialMenu = {
	.id = MENU_ACT_SOCIAL,
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawSocialMenu,
	.options = {
			{&activitiesMenu, {NULL, NULL, NULL}, "Takaisin"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

Menu activitiesMenu = {
	.id = MENU_ACTIVITIES,
	.numOptions = 5,
	.selectedOption = 0,
	.drawFxn = DrawActivitiesMenu,
	.options = {
			{&mainMenu, {NULL, NULL, NULL}, "Takaisin"},
			{&sunMenu, {Menu_StartRedrawing, Sensors_StartTrackingSun, NULL}, "Aurinko"},
			{&airMenu, {Menu_StartRedrawing, Sensors_StartTrackingFreshAir, NULL}, "Raikas ilma"},
			{&physMenu, {Menu_StartRedrawing, Sensors_StartTrackingPhysical, NULL}, "Liikunta"},
			{&socialMenu, {NULL, NULL, NULL}, "Kaverit"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

Menu mainMenu = {
	.id = MENU_MAIN,
	.numOptions = 4,
	.selectedOption = 0,
	.drawFxn = DrawMainMenu,
	.options = {
			{NULL, {Comm_CreateNewGotchi, NULL, NULL}, "Luo uusi"},
			{NULL, {NULL, NULL, NULL}, "Hae/vie"},
			{&activitiesMenu, {NULL, NULL, NULL}, "Harrasta"},
			{NULL, {NULL, NULL, NULL}, "Sammuta"},
			{NULL, {NULL, NULL, NULL}, ""}
	}
};

const Menu *GetFirstMenu()
{
	return &mainMenu;
}

int isLastOption(const MenuOption opt)
{
	return opt.actions[0] == NULL && opt.next == NULL && !strlen(opt.text);
}
