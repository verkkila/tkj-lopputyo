/*
 * sensor_task.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include <sensors/sensor_main.h>

#define SENSORS_STACKSIZE 2048
Char sensorStack[SENSORS_STACKSIZE];

I2C_Handle hI2C;
I2C_Handle *pI2C = NULL;
I2C_Handle *pMpuI2C = NULL;
static I2C_Params i2cParams;
static I2C_Params i2cMpuParams;

static PIN_Handle hMpuPin;
static PIN_State sMpuPin;
static PIN_Config MpuPinConfig[] = {
		Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
		.pinSDA = Board_I2C0_SDA1,
		.pinSCL = Board_I2C0_SCL1
};

static Clock_Handle		Conversion_Clock;
static Clock_Handle		TMP007_Clock;
static Clock_Handle		BMP280_Clock;
static Clock_Handle		OPT3001_Clock;
static Clock_Handle 	MPU9250_Clock;
static Semaphore_Handle	convertValues;

int TMP007_numData = 0;
int BMP280_numData = 0;
int OPT3001_numData = 0;
int MPU9250_numData = 0;

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

Void I2C_MPU9250_CompleteFxn(I2C_Handle handle, I2C_Transaction *msg, Bool transfer)
{
	if (transfer)
		MPU9250_TransferComplete(msg);
	else {
		System_printf("MPU9250 I2C failed.\n");
		System_flush();
	}
}

void Sensors_SwitchToNormalI2C()
{
	if (i2cMode == I2CMODE_NORMAL)
		return;
	if (*pMpuI2C == NULL)
		return;
	I2C_close(*pMpuI2C);
	pMpuI2C = NULL;
	hI2C = I2C_open(Board_I2C0, &i2cParams);
	pI2C = &hI2C;
	i2cMode = I2CMODE_NORMAL;
}

void Sensors_SwitchToMPU9250I2C()
{
	if (i2cMode == I2CMODE_MPU9250)
		return;
	if (*pI2C == NULL)
		return;
	I2C_close(*pI2C);
	pI2C = NULL;
	hI2C = I2C_open(Board_I2C, &i2cMpuParams);
	pMpuI2C = &hI2C;
	i2cMode = I2CMODE_MPU9250;
}

Void TMP007_Tick(UArg arg)
{
	TMP007_Read();
}

Void BMP280_Tick(UArg arg)
{
	BMP280_Read();
}

Void OPT3001_Tick(UArg arg)
{
	OPT3001_Read();
}

Void MPU9250_Tick(UArg arg)
{
	MPU9250_Read();
}


Void Conversion_Clock_Tick(UArg arg)
{
	Semaphore_post(convertValues);
}

static void AccumulateSun(void)
{
	int i, max_index;
	float temp_a = 0;

	max_index = TMP007_numData < OPT3001_numData ? TMP007_numData : OPT3001_numData;
	for (i = 0; i < max_index; ++i) {
		if (TMP007_data[i] > TEMPERATURE_THRESHOLD && OPT3001_data[i] > LUMINOSITY_THRESHOLD) {
			temp_a += TMP007_READ_RATE_MS;
		}
	}
	temp_a *= 0.001f;
	currentGotchi.a += floor(temp_a);
}

void Sensors_StartTrackingSun(void)
{
	Sensors_SwitchToNormalI2C();
	Clock_start(Conversion_Clock);
	Clock_start(TMP007_Clock);
	Clock_start(OPT3001_Clock);
	trackingMode = TRACKING_SUN;
	System_printf("Started tracking sun.\n");
	System_flush();
}

void Sensors_StopTrackingSun(void)
{
	trackingMode = TRACKING_NONE;
	TMP007_ConvertData();
	OPT3001_ConvertData();
	AccumulateSun();
	Clock_stop(TMP007_Clock);
	Clock_stop(OPT3001_Clock);
	Clock_stop(Conversion_Clock);
	System_printf("Stopped tracking sun.\n");
	System_flush();
}

static void AccumulateFreshAir(void)
{
	int i;
	float temp_r = 0;

	for (i = 0; i < BMP280_numData; ++i) {
		if (BMP280_presData[i] < GROUND_AIR_PRESSURE - PRESSURE_DIFFERENCE) {
			temp_r += BMP280_READ_RATE_MS;
		}
	}
	temp_r *= 0.001f;
	currentGotchi.r += floor(temp_r);
}

void Sensors_StartTrackingFreshAir(void)
{
	Sensors_SwitchToNormalI2C();
	Clock_start(Conversion_Clock);
	Clock_start(BMP280_Clock);
	trackingMode = TRACKING_FRESH_AIR;
	System_printf("Started tracking fresh air.\n");
	System_flush();
}

void Sensors_StopTrackingFreshAir(void)
{
	trackingMode = TRACKING_NONE;
	BMP280_ConvertData();
	AccumulateFreshAir();
	Clock_stop(BMP280_Clock);
	Clock_stop(Conversion_Clock);
	System_printf("Stopped tracking fresh air.\n");
	System_flush();
}

static void AccumulatePhysicalActivity(void)
{
	int i;
	float temp_l = 0;

	for (i = 0; i < MPU9250_numData; ++i) {
		if (vec3f_GetLength(&MPU9250_data[i]) > MOVEMENT_THRESHOLD) {
			temp_l += MPU9250_READ_RATE_MS;
		}
	}
	temp_l *= 0.001f;
	currentGotchi.l += floor(temp_l);
}

void Sensors_StartTrackingPhysical(void)
{
	Sensors_SwitchToMPU9250I2C();
	trackingMode = TRACKING_PHYSICAL;
	Clock_start(Conversion_Clock);
	Clock_start(MPU9250_Clock);
	System_printf("Started tracking physical.\n");
	System_flush();
}

void Sensors_StopTrackingPhysical(void)
{
	trackingMode = TRACKING_PHYSICAL;
	AccumulatePhysicalActivity();
	Clock_stop(MPU9250_Clock);
	Clock_stop(Conversion_Clock);
	System_printf("Stopped tracking physical.\n");
	System_flush();
}


static void SetI2CParams(void)
{
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_400kHz;
	i2cParams.transferMode = I2C_MODE_CALLBACK;
	i2cParams.transferCallbackFxn = (I2C_CallbackFxn)I2C_CompleteFxn;

	I2C_Params_init(&i2cMpuParams);
	i2cMpuParams.bitRate = I2C_400kHz;
	i2cMpuParams.transferMode = I2C_MODE_CALLBACK;
	i2cMpuParams.transferCallbackFxn = (I2C_CallbackFxn)I2C_MPU9250_CompleteFxn;
	i2cMpuParams.custom = (uintptr_t)&i2cMPUCfg;
}


Void Sensors_Update(UArg arg0, UArg arg1)
{
	I2C_Handle		i2c;
	I2C_Handle		i2cMpu;
	Clock_Params	Master_Params;
	Clock_Params	TMP007_Params;
	Clock_Params	BMP280_Params;
	Clock_Params	OPT3001_Params;
	Clock_Params	MPU9250_Params;
	Semaphore_Params	convertValuesParams;

	i2cMode = I2CMODE_NONE;
	trackingMode = TRACKING_NONE;

	Clock_Params_init(&TMP007_Params);
	TMP007_Params.period = TMP007_READ_RATE_MS * 1000 / Clock_tickPeriod;
	TMP007_Clock = Clock_create(TMP007_Tick, 0, &TMP007_Params, NULL);

	Clock_Params_init(&BMP280_Params);
	BMP280_Params.period = BMP280_READ_RATE_MS * 1000 / Clock_tickPeriod;
	BMP280_Clock = Clock_create(BMP280_Tick, 0, &BMP280_Params, NULL);

	Clock_Params_init(&OPT3001_Params);
	OPT3001_Params.period = OPT3001_READ_RATE_MS * 1000 / Clock_tickPeriod;
	OPT3001_Clock = Clock_create(OPT3001_Tick, 0, &OPT3001_Params, NULL);

	Clock_Params_init(&MPU9250_Params);
	MPU9250_Params.period = MPU9250_READ_RATE_MS * 1000 / Clock_tickPeriod;
	MPU9250_Clock = Clock_create(MPU9250_Tick, 0, &MPU9250_Params, NULL);

	Clock_Params_init(&Master_Params);
	Master_Params.period = CONVERSION_RATE_MS * 1000 / Clock_tickPeriod;
	Conversion_Clock = Clock_create(Conversion_Clock_Tick, CONVERSION_RATE_MS * 1000 / Clock_tickPeriod, &Master_Params, NULL);

	if (TMP007_Clock == NULL ||
		BMP280_Clock == NULL ||
		OPT3001_Clock == NULL ||
		MPU9250_Clock == NULL ||
		Conversion_Clock == NULL) {
		System_abort("Failed to create sensor clocks.");
	}

	Semaphore_Params_init(&convertValuesParams);
	convertValues = Semaphore_create(0, &convertValuesParams, NULL);
	if (!convertValues) {
		System_abort("Failed to create semaphore");
	}

	SetI2CParams();

	i2c = I2C_open(Board_I2C0, &i2cParams);
	i2cMode = I2CMODE_NORMAL;
	if (i2c == NULL) {
		System_abort("Error Initializing I2C (normal)\n");
	} else {
		System_printf("I2C Initialized! (normal)\n");
	}

	pI2C = &i2c;

    TMP007_Setup();
    OPT3001_Setup(&i2c);
    BMP280_Setup(&i2c);
    Event_pend(globalEvents, SENSOR_SETUP_COMPLETE, Event_Id_NONE, BIOS_WAIT_FOREVER);

    I2C_close(i2c);
    i2cMode = I2CMODE_NONE;
    pI2C = NULL;

    hMpuPin = PIN_open(&sMpuPin, MpuPinConfig);
    if (hMpuPin == NULL) {
    	System_abort("Failed to open MPU9250 pin.");
    }

    i2cMpu = I2C_open(Board_I2C, &i2cMpuParams);
    i2cMode = I2CMODE_MPU9250;
    if (i2cMpu == NULL) {
		System_abort("Error Initializing I2C (MPU9250)\n");
	} else {
		System_printf("I2C Initialized! (MPU9250)\n");
	}

    pMpuI2C = &i2cMpu;
    PIN_setOutputValue(hMpuPin, Board_MPU_POWER, Board_MPU_POWER_ON);
    Task_sleep(100 * 1000 / Clock_tickPeriod);

    MPU9250_Setup(&i2cMpu);

    while (1) {
    	Semaphore_pend(convertValues, BIOS_WAIT_FOREVER);
		switch (trackingMode) {
		case TRACKING_SUN:
			Clock_stop(TMP007_Clock);
			Clock_stop(OPT3001_Clock);

			TMP007_ConvertData();
			OPT3001_ConvertData();

			AccumulateSun();

			TMP007_numData = 0;
			OPT3001_numData = 0;

			Clock_start(TMP007_Clock);
			Clock_start(OPT3001_Clock);
			break;
		case TRACKING_FRESH_AIR:
			Clock_stop(BMP280_Clock);
			BMP280_ConvertData();
			AccumulateFreshAir();
			BMP280_numData = 0;
			Clock_start(BMP280_Clock);
			break;
		case TRACKING_PHYSICAL:
			Clock_stop(MPU9250_Clock);
			AccumulatePhysicalActivity();
			MPU9250_numData = 0;
			Clock_start(MPU9250_Clock);
			break;
		case TRACKING_NONE:
		default:
			break;
		}
		Event_post(globalEvents, DATA_CONVERSION_COMPLETE);
    }
}

Void Sensors_CreateTask(void)
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = SENSORS_STACKSIZE;
	params.stack = &sensorStack;
	params.priority = 3;

	task = Task_create(Sensors_Update, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create sensor task!");
	}
}
