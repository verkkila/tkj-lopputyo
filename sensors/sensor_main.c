/*
 * sensor_task.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include <sensors/sensor_main.h>

#define SENSORS_STACKSIZE 4096
Char sensorStack[SENSORS_STACKSIZE];

I2C_Handle hI2C;

sensor_display_data sensorDisplayData;

uint32_t a = 0;
uint32_t r = 0;

Void timer250_Tick(UArg arg)
{

}

Void timer500_Tick(UArg arg)
{
	Clock_tick();
	System_printf("500ms\n");
	System_flush();
}

Void timer1000_Tick(UArg arg)
{
	//laske yhteen kaikki aktiivisuustiedot
	Clock_tick();
	System_printf("1000ms\n");
	System_flush();
}

Void Sensors_TrackSun()
{
	static float oldLuminosity = 0.f, oldTemperature = 0.f;
	float newLuminosity, newTemperature;
	newLuminosity = OPT3001_GetLuminosity(&hI2C);
	newTemperature = TMP007_GetTemperature(&hI2C);
	if (oldLuminosity > 25.f && newLuminosity > 25.f  &&
			oldTemperature > 30.f && newTemperature > 30.f) {
			++a;
		}
	sprintf(sensorDisplayData.TMP007, "T:%.2f a: %i", newTemperature, a);
	sprintf(sensorDisplayData.OPT3001, "L:%.2f", newLuminosity);

	oldLuminosity = newLuminosity;
	oldTemperature = newTemperature;
}

Void Sensors_TrackFreshAir()
{
	static float oldPressure = 0.f;
	float newPressure;
	newPressure = BMP280_GetPressure(&hI2C);
	if (oldPressure > 109339.0f && newPressure > 109339.f) {
		++r;
	}
	sprintf(sensorDisplayData.BMP280, "P:%.2f r:%i", newPressure, r);
	oldPressure = newPressure;
}

Void Sensors_ReadAll(UArg arg0, UArg arg1)
{
	Clock_Handle	clock250;
	Clock_Handle	clock500;
	Clock_Handle	clock1000;
	Clock_Params	testClockParams;
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
	hI2C = i2c;
	Clock_Params_init(&testClockParams);
	testClockParams.period = Clock_tickPeriod;
	clock250 = Clock_create(timer250_Tick, 0, &testClockParams, NULL);
	testClockParams.period = 500000 / Clock_tickPeriod;
	clock500 = Clock_create(timer500_Tick, 0, &testClockParams, NULL);
	testClockParams.period = 1000000 / Clock_tickPeriod;
	clock1000 = Clock_create(timer1000_Tick, 0, &testClockParams, NULL);
    if (clock250 == NULL ||
    	clock500 == NULL ||
		clock1000 == NULL) {
    	System_abort("Failed to create clocks!");
    }
	BMP280_Setup(&i2c);
    TMP007_Setup(&i2c);
    OPT3001_Setup(&i2c);

    /*
    Clock_start(clock250);
    Clock_start(clock500);
    Clock_start(clock1000);
    */
    while (1) {
    	Sensors_TrackSun();
    	Sensors_TrackFreshAir();
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
