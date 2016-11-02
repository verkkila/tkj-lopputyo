/*
 * sensor_task.h
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#ifndef SENSORS_SENSOR_MAIN_H_
#define SENSORS_SENSOR_MAIN_H_

#include <stdio.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/I2C.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>

#include "../Board.h"
#include "../eventdefs.h"
#include "sensordefs.h"
#include "bmp280.h"
#include "hdc1000.h"
#include "mpu9250.h"
#include "opt3001.h"
#include "tmp007.h"

extern Event_Handle g_hEvent;

Void Sensors_Start();

#endif /* SENSORS_SENSOR_MAIN_H_ */
