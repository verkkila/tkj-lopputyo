/*
 * comm_main.h
 *
 *  Created on: Dec 5, 2016
 *      Author: tsei
 */

#ifndef WIRELESS_COMM_MAIN_H_
#define WIRELESS_COMM_MAIN_H_

#include <stdio.h>
#include <string.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include "comm_lib.h"
#include "../aasigotchi.h"

extern aasigotchi_data *currentGotchi;

void Comm_Start(void);
void Comm_CreateNewGotchi(void);
void Comm_ParseReturnMsg(void);
void Comm_FetchGotchi(void);

#endif /* WIRELESS_COMM_MAIN_H_ */
