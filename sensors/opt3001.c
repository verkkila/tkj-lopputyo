/*
 * opt3001.c
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/opt3001.pdf
 */

#include "opt3001.h"

static I2C_Transaction writeConfig;
static I2C_Transaction readConfig;
static I2C_Transaction readResult;
static Bool isDataReady;
static char txBuffer[4];
static char rxBuffer[4];

static uint16_t rawData[OPT3001_NUM_VALUES];
static float data[OPT3001_NUM_VALUES];
static int opt3001_index = 0;

void OPT3001_Setup(I2C_Handle *i2c)
{
	writeConfig.slaveAddress = Board_OPT3001_ADDR;
    txBuffer[0] = OPT3001_REG_CONFIG;
    txBuffer[1] = 0xCE; // continuous mode s.22
    txBuffer[2] = 0x02;
    writeConfig.writeBuf = txBuffer;
    writeConfig.writeCount = 3;
    writeConfig.readBuf = NULL;
    writeConfig.readCount = 0;

    I2C_transfer(*pI2C, &writeConfig);
}

static void OPT3001_readDataState(char *buf)
{
	uint16_t e;
	e = (buf[0] << 8) | buf[1];
	isDataReady = (e & OPT3001_DATA_READY);
}

static void OPT3001_AddData(char *buf)
{
	//System_printf("BMP280: AddData index: %i tick: %i\n", bmp280_index, Clock_getTicks() * Clock_tickPeriod / 1000);
	System_flush();
	if (opt3001_index >= OPT3001_NUM_VALUES) {
		System_printf("OPT3001 raw data buffer is full, waiting for conversion.\n");
	} else {
		rawData[opt3001_index] = (buf[0] << 8) | buf[1];
		++opt3001_index;
	}
}

void OPT3001_HandleMsg(I2C_Transaction *msg, Bool transfer)
{
	uint8_t reg = *(uint8_t*)msg->writeBuf;
	switch (reg) {
	case OPT3001_REG_CONFIG:
		OPT3001_readDataState(msg->readBuf);
		break;
	case OPT3001_REG_RESULT:
		if (isDataReady) {
			OPT3001_AddData(msg->readBuf);
		}
		isDataReady = false;
		break;
	default:
		break;
	}
}

void OPT3001_Read()
{
	txBuffer[0] = OPT3001_REG_CONFIG;
	readConfig.slaveAddress = Board_OPT3001_ADDR;
	readConfig.writeBuf = txBuffer;
	readConfig.writeCount = 1;
	readConfig.readBuf = rxBuffer;
	readConfig.readCount = 2;

	txBuffer[1] = OPT3001_REG_RESULT;
	readResult.slaveAddress = Board_OPT3001_ADDR;
	readResult.writeBuf = &txBuffer[1];
	readResult.writeCount = 1;
	readResult.readBuf = &rxBuffer[2];
	readResult.readCount = 2;

	I2C_transfer(*pI2C, &readConfig);
	I2C_transfer(*pI2C, &readResult);
}

static float OPT3001_ConvertLuminosity(uint16_t luminosityRaw)
{
	float lux = 0.f;
	int exp = luminosityRaw >> 12;
	int factor = luminosityRaw & 0x0FFF;
	lux = 0.01 * pow2(exp) * factor;
	return lux;
}

void OPT3001_ConvertData()
{
	int i;

	System_printf("OPT3001: Starting conversion, index: (%i/%i)\n", opt3001_index, OPT3001_NUM_VALUES);
	for (i = 0; i <= opt3001_index; ++i) {
		data[i] = OPT3001_ConvertLuminosity(rawData[i]);
	}
	opt3001_index = 0;
	System_printf("OPT3001 conversion complete.\n");
}

/*
float OPT3001_GetLuminosity() {
	uint16_t e = 0;
	uint16_t luminosityRaw = 0;
	float lux = 0;

	i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
	txBuffer[0] = OPT3001_REG_CONFIG;
	i2cTransaction.writeBuf = txBuffer;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = rxBuffer;
	i2cTransaction.readCount = 2;

	if (I2C_transfer(*pI2C, &i2cTransaction)) {
		e = (rxBuffer[0] << 8) | rxBuffer[1];
	} else {
		e = 0;
		System_printf("OPT3001: Config read failed!\n");
		System_flush();
	}

	if (e & OPT3001_DATA_READY) {
		txBuffer[0] = OPT3001_REG_RESULT;
	    i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
	    i2cTransaction.writeBuf = txBuffer;
	    i2cTransaction.writeCount = 1;
	    i2cTransaction.readBuf = rxBuffer;
	    i2cTransaction.readCount = 2;

		if (I2C_transfer(*pI2C, &i2cTransaction)) {
			luminosityRaw = (rxBuffer[0] << 8) | rxBuffer[1];
			int exp = luminosityRaw >> 12;
			int factor = luminosityRaw & 0x0FFF;
			lux = 0.01 * pow2(exp) * factor;
		} else {
			System_printf("OPT3001: Data read failed!\n");
			System_flush();
		}
	}
	return lux;
}
*/
