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
Bool isDataReady;
static char txBuffer[4];
static char rxBuffer[4];

static uint16_t rawData[OPT3001_NUM_VALUES];
float OPT3001_data[OPT3001_NUM_VALUES];

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

    I2C_transfer(*i2c, &writeConfig);
}

static void OPT3001_readDataState(char *buf)
{
	uint16_t e;
	e = (buf[0] << 8) | buf[1];
	isDataReady = (e & OPT3001_DATA_READY);
}

static void OPT3001_AddData(char *buf)
{
	if (OPT3001_numData >= OPT3001_NUM_VALUES) {
		System_printf("OPT3001 raw data buffer is full, waiting for conversion.\n");
	} else {
		rawData[OPT3001_numData] = (buf[0] << 8) | buf[1];
		++OPT3001_numData;
	}
	System_flush();
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
	for (i = 0; i < OPT3001_numData; ++i) {
		OPT3001_data[i] = OPT3001_ConvertLuminosity(rawData[i]);
	}
}
