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
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include "../Board.h"
#include "../eventdefs.h"
#include "../aasigotchi.h"
#include "sensordefs.h"
#include "bmp280.h"
#include "mpu9250.h"
#include "opt3001.h"
#include "tmp007.h"

extern Event_Handle globalEvents;
extern aasigotchi_data currentGotchi;

enum I2CMODE {
	I2CMODE_NONE = 0,
	I2CMODE_NORMAL,
	I2CMODE_MPU9250
};
enum I2CMODE i2cMode;

enum TRACKING_MODE {
	TRACKING_NONE,
	TRACKING_SUN,
	TRACKING_FRESH_AIR,
	TRACKING_PHYSICAL,
};
enum TRACKING_MODE trackingMode;

Void Sensors_CreateTask(void);

void Sensors_StartTrackingSun(void);
void Sensors_StartTrackingFreshAir(void);
void Sensors_StartTrackingPhysical(void);

void Sensors_StopTrackingSun(void);
void Sensors_StopTrackingFreshAir(void);
void Sensors_StopTrackingPhysical(void);

#endif /* SENSORS_SENSOR_MAIN_H_ */
