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

#include "../Board.h"
#include "bmp280.h"
#include "hdc1000.h"
#include "mpu9250.h"
#include "opt3001.h"
#include "tmp007.h"

#define DATA_READ_COMPLETE Event_Id_00

extern Event_Handle g_hEvent;

typedef struct {
	char TMP007[16];
	char OPT3001[16];
	char BMP280[16];
} sensor_display_data;

extern sensor_display_data sensorDisplayData;

Void Sensors_Start();

#endif /* SENSORS_SENSOR_MAIN_H_ */
