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
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "menu_impl.h"
#include "../sensors/sensor_main.h"
#include "../aasigotchi.h"
#include "../util/vec2f.h"

extern aasigotchi_data *currentGotchi;

Void powerButtonFxn(PIN_Handle handle, PIN_Id pinId);

Void Menu_StartRedrawing(void);
Void Menu_StopRedrawing(void);

Void Menu_StartS(void);
Void Menu_StartA(void);
Void Menu_StartP(void);

Void Menu_StopS(void);
Void Menu_StopA(void);
Void Menu_StopP(void);

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id);
Void Menu_OnButton1(PIN_Handle handle, PIN_Id id);

Void DrawMainMenu(tContext *pContext);
Void DrawActivitiesMenu(tContext *pContext);
Void DrawSunMenu(tContext *pContext);
Void DrawAirMenu(tContext *pContext);
Void DrawPhysMenu(tContext *pContext);
Void DrawSocialMenu(tContext *pContext);

Void Menu_Start();

#endif /* DISPLAY_MENU_H_ */
