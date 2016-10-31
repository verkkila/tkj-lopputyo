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

#include "../sensors/sensor_main.h"

Void Menu_Start();

#endif /* DISPLAY_MENU_H_ */
