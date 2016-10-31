/*
 * sensor_task.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include <sensors/sensor_main.h>

#define SENSORS_STACKSIZE 4096
Char sensorStack[SENSORS_STACKSIZE];
sensor_display_data sensorDisplayData;

Void Sensors_ReadAll(UArg arg0, UArg arg1)
{
	I2C_Handle		i2c;
	I2C_Params      i2cParams;
	/* Create I2C for usage */
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_400kHz;
	i2c = I2C_open(Board_I2C0, &i2cParams);
	if (i2c == NULL) {
		System_abort("Error Initializing I2C\n");
	} else {
		System_printf("I2C Initialized!\n");
	}
    BMP280_Setup(&i2c);
    TMP007_Setup(&i2c);
    OPT3001_Setup(&i2c);

    while (1) {
    	float temp = TMP007_GetTemperature(&i2c);
		float lum = OPT3001_GetLuminosity(&i2c);
		float pressure = BMP280_GetPressure(&i2c);
		sprintf(sensorDisplayData.TMP007, "T:%.2f", temp);
		sprintf(sensorDisplayData.OPT3001, "L:%.2f", lum);
		sprintf(sensorDisplayData.BMP280, "P:%.2f", pressure);
		Event_post(g_hEvent, DATA_READ_COMPLETE);
		Task_sleep(1000000 / Clock_tickPeriod);
    }
}

Void Sensors_CreateTask()
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = SENSORS_STACKSIZE;
	params.stack = &sensorStack;
	params.priority = 3;

	task = Task_create(Sensors_ReadAll, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create sensor task!");
	}
}

Void Sensors_Start()
{
	Sensors_CreateTask();
}
