/*
 * menu.h
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#ifndef DISPLAY_MENU_H_
#define DISPLAY_MENU_H_

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "../sensors/sensor_main.h"
#include "../aasigotchi.h"

enum MainMenuOptions {
	MM_OPT_CREATENEW = 0,
	MM_OPT_LOAD,
	MM_OPT_ACTIVITIES,
	MM_OPT_HELP
};

enum SubMenu1Options {
	SM1_OPT_RETURN = 0,
	SM1_OPT_SUN,
	SM1_OPT_FRESHAIR,
	SM1_OPT_PHYS,
	SM1_OPT_HELLO
};

enum MenuId {
	MENU_MAIN,
	MENU_SUB1
};

extern PIN_Handle hLed;
extern aasigotchi_data *currentGotchi;

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id);
Void Menu_OnButton1(PIN_Handle handle, PIN_Id id);

Void Menu_Start();

#endif /* DISPLAY_MENU_H_ */
