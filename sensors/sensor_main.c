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

int tmp007_index = 0;
int bmp280_index = 0;
int opt3001_index = 0;

float a = 0.f, r = 0.f, l = 0.f;

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
	Event_post(g_hEvent, START_CONVERSIONS);
	/*
	*/
}

static void AccumulateSun()
{
	int i, max_index;

	max_index = tmp007_index < opt3001_index ? tmp007_index : opt3001_index;
	for (i = 0; i < max_index; ++i) {
		if (TMP007_data[i] > TEMPERATURE_THRESHOLD && OPT3001_data[i] > LUMINOSITY_THRESHOLD) {
			a += TMP007_READ_RATE_MS / 1000.f;
			System_printf("Auringonottoa...\n", i);
			System_flush();
		}
	}
}

static void AccumulateFreshAir()
{
	int i;

	for (i = 0; i < bmp280_index; ++i) {
		if (BMP280_presData[i] < 109114.f) {
			r += BMP280_READ_RATE_MS / 1000.f;
			System_printf("Raikasta ilmaa...\n", i);
			System_flush();
		}
	}
}

static void AccumulatePhysicalActivity()
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
    BMP280_Setup(&i2c);

    Event_pend(g_hEvent, SENSOR_SETUP_COMPLETE, Event_Id_NONE, BIOS_WAIT_FOREVER);

    Clock_start(TMP007_Clock);
    Clock_start(BMP280_Clock);
    Clock_start(OPT3001_Clock);
    Clock_start(Master_Clock);

    while (1) {
    	Event_pend(g_hEvent, START_CONVERSIONS, Event_Id_NONE, BIOS_WAIT_FOREVER);
		System_printf("Converting values.\n");

		Clock_stop(TMP007_Clock);
		Clock_stop(OPT3001_Clock);
		Clock_stop(BMP280_Clock);

		TMP007_ConvertData();
		OPT3001_ConvertData();
		BMP280_ConvertData();

		AccumulateFreshAir();
		AccumulateSun();

		tmp007_index = 0;
		bmp280_index = 0;
		opt3001_index = 0;

		Clock_start(TMP007_Clock);
		Clock_start(OPT3001_Clock);
		Clock_start(BMP280_Clock);

		Event_post(g_hEvent, DATA_CONVERSION_COMPLETE);
		System_flush();
		//Task_sleep(1000000 / Clock_tickPeriod);
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
