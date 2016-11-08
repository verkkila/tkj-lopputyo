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

#include "menu_impl.h"
#include "../sensors/sensor_main.h"
#include "../aasigotchi.h"

extern aasigotchi_data *currentGotchi;

Void powerButtonFxn(PIN_Handle handle, PIN_Id pinId);

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id);
Void Menu_OnButton1(PIN_Handle handle, PIN_Id id);

Void DrawMainMenu(tContext *pContext);
Void DrawActivitiesMenu(tContext *pContext);
Void DrawSunMenu(tContext *pContext);

void dummyFxn1(void);
void dummyFxn2(void);
void dummyFxn3(void);
void dummyFxn4(void);

Void Menu_Start();

#endif /* DISPLAY_MENU_H_ */
