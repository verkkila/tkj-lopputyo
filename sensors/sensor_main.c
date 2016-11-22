/*
 * sensor_task.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include <sensors/sensor_main.h>

#define SENSORS_STACKSIZE 4096
Char sensorStack[SENSORS_STACKSIZE];

I2C_Handle *pI2C = NULL;

Clock_Handle	Master_Clock;
Clock_Handle	TMP007_Clock;
Clock_Handle	BMP280_Clock;
Clock_Handle	OPT3001_Clock;
Semaphore_Handle	sem;

int tmp007_index = 0;
int bmp280_index = 0;
int opt3001_index = 0;

int isTrackingSun = 0;
int isTrackingFreshAir = 0;
int isTrackingPhysical = 0;

Void I2C_CompleteFxn(I2C_Handle handle, I2C_Transaction *msg, Bool transfer)
{
	switch (msg->slaveAddress) {
	case Board_TMP007_ADDR:
		TMP007_HandleMsg(msg, transfer);
		break;
	case Board_OPT3001_ADDR:
		OPT3001_HandleMsg(msg, transfer);
		break;
	case Board_BMP280_ADDR:
		BMP280_HandleMsg(msg, transfer);
		break;
	default:
		break;
	}
}

Void TMP007_Tick(UArg arg)
{
	TMP007_Read();
	//System_printf("TMP007 tick %i\n", Clock_getTicks());
	//System_flush();
}

Void BMP280_Tick(UArg arg)
{
	BMP280_Read();
	//System_printf("BMP280 ticks %i\n", Clock_getTicks());
	//System_flush();
}

Void OPT3001_Tick(UArg arg)
{
	OPT3001_Read();
	//System_printf("OPT3001 ticks %i\n", Clock_getTicks());
	//System_flush();
}


Void Master_Clock_Tick(UArg arg)
{
	//Event_post(g_hEvent, START_CONVERSIONS);
	Semaphore_post(sem);
	/*
	*/
}

static void AccumulateSun(void)
{
	int i, max_index;
	float temp_a;

	max_index = tmp007_index < opt3001_index ? tmp007_index : opt3001_index;
	for (i = 0; i < max_index; ++i) {
		if (TMP007_data[i] > TEMPERATURE_THRESHOLD && OPT3001_data[i] > LUMINOSITY_THRESHOLD) {
			temp_a += TMP007_READ_RATE_MS;
		}
	}
	temp_a *= 0.001f;
	currentGotchi->a += floor(temp_a);
}

void Sensors_StartTrackingSun(void)
{
	Clock_start(TMP007_Clock);
	Clock_start(OPT3001_Clock);
	Clock_start(Master_Clock);
	isTrackingSun = 1;
	System_printf("Started tracking sun.\n");
	System_flush();
}

void Sensors_StopTrackingSun(void)
{
	isTrackingSun = 0;
	TMP007_ConvertData();
	OPT3001_ConvertData();
	AccumulateSun();
	Clock_stop(TMP007_Clock);
	Clock_stop(OPT3001_Clock);
	Clock_stop(Master_Clock);
	System_printf("Stopped tracking sun.\n");
	System_flush();
}

static void AccumulateFreshAir(void)
{
	int i;
	float temp_r;

	for (i = 0; i < bmp280_index; ++i) {
		if (BMP280_presData[i] < PRESSURE_THRESHOLD) {
			temp_r += BMP280_READ_RATE_MS;
		}
	}
	temp_r *= 0.001f;
	currentGotchi->r += floor(temp_r);
}

void Sensors_StartTrackingFreshAir(void)
{
	Clock_start(BMP280_Clock);
	Clock_start(Master_Clock);
	isTrackingFreshAir = 1;
}

void Sensors_StopTrackingFreshAir(void)
{
	isTrackingFreshAir = 0;
	BMP280_ConvertData();
	AccumulateFreshAir();
	Clock_stop(BMP280_Clock);
	Clock_stop(Master_Clock);
}

void Sensors_StartTrackingPhysical(void)
{

}

void Sensors_StopTrackingPhysical(void)
{

}

static void AccumulatePhysicalActivity(void)
{

}

Void Sensors_ReadAll(UArg arg0, UArg arg1)
{
	I2C_Handle		i2c;
	I2C_Params      i2cParams;
	Clock_Params	Master_Params;
	Clock_Params	TMP007_Params;
	Clock_Params	BMP280_Params;
	Clock_Params	OPT3001_Params;
	Semaphore_Params	semParams;

	Clock_Params_init(&TMP007_Params);
	TMP007_Params.period = TMP007_READ_RATE_MS * 1000 / Clock_tickPeriod;
	TMP007_Clock = Clock_create(TMP007_Tick, 0, &TMP007_Params, NULL);

	Clock_Params_init(&BMP280_Params);
	BMP280_Params.period = BMP280_READ_RATE_MS * 1000 / Clock_tickPeriod;
	BMP280_Clock = Clock_create(BMP280_Tick, 0, &BMP280_Params, NULL);

	Clock_Params_init(&OPT3001_Params);
	OPT3001_Params.period = OPT3001_READ_RATE_MS * 1000 / Clock_tickPeriod;
	OPT3001_Clock = Clock_create(OPT3001_Tick, 0, &OPT3001_Params, NULL);

	Clock_Params_init(&Master_Params);
	Master_Params.period = CONVERSION_RATE_MS * 1000 / Clock_tickPeriod;
	Master_Clock = Clock_create(Master_Clock_Tick, CONVERSION_RATE_MS * 1000 / Clock_tickPeriod, &Master_Params, NULL);

	if (TMP007_Clock == NULL ||
		BMP280_Clock == NULL ||
		OPT3001_Clock == NULL ||
		Master_Clock == NULL) {
		System_abort("Failed to create sensor clocks.");
	}

	Semaphore_Params_init(&semParams);
	sem = Semaphore_create(0, &semParams, NULL);
	if (!sem) {
		System_abort("Failed to create semaphore");
	}
	/* Create I2C for usage */
	I2C_Params_init(&i2cParams);
	i2cParams.transferMode = I2C_MODE_CALLBACK;
	i2cParams.transferCallbackFxn = (I2C_CallbackFxn)I2C_CompleteFxn;
	i2cParams.bitRate = I2C_400kHz;
	i2c = I2C_open(Board_I2C0, &i2cParams);
	if (i2c == NULL) {
		System_abort("Error Initializing I2C\n");
	} else {
		System_printf("I2C Initialized!\n");
	}
	pI2C = &i2c;

    TMP007_Setup(&i2c);
    OPT3001_Setup(&i2c);
    //BMP280_Setup(&i2c);

    //Event_pend(g_hEvent, SENSOR_SETUP_COMPLETE, Event_Id_NONE, BIOS_WAIT_FOREVER);

    while (1) {
    	Semaphore_pend(sem, BIOS_WAIT_FOREVER);
		System_printf("Converting values.\n");
		if (isTrackingSun) {
			Clock_stop(TMP007_Clock);
			Clock_stop(OPT3001_Clock);

			TMP007_ConvertData();
			OPT3001_ConvertData();

			AccumulateSun();

			tmp007_index = 0;
			opt3001_index = 0;

			Clock_start(TMP007_Clock);
			Clock_start(OPT3001_Clock);
		} else if (isTrackingFreshAir) {
			Clock_stop(BMP280_Clock);
			BMP280_ConvertData();
			AccumulateFreshAir();
			bmp280_index = 0;
			Clock_start(BMP280_Clock);
		} else if (isTrackingPhysical) {

		}
		Event_post(g_hEvent, DATA_CONVERSION_COMPLETE);
		System_flush();
    }
}

Void Sensors_CreateTask(void)
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = SENSORS_STACKSIZE;
	params.stack = &sensorStack;
	params.priority = 1;

	task = Task_create(Sensors_ReadAll, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create sensor task!");
	}
}

Void Sensors_Start(void)
{
	Sensors_CreateTask();
}
