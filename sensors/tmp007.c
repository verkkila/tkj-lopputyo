/*
 * tmp007.c
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */
#include "tmp007.h"

static I2C_Transaction readData;
static char txBuffer[1];
static char rxBuffer[2];

uint16_t rawData[TMP007_NUM_VALUES];
float TMP007_data[TMP007_NUM_VALUES];
//int tmp007_numData;

void TMP007_Setup()
{
	System_printf("TMP007: Config not needed!\n");
    System_flush();
}

void TMP007_Read()
{
	txBuffer[0] = TMP007_REG_TEMP;
	readData.slaveAddress = Board_TMP007_ADDR;
	readData.writeBuf = txBuffer;
	readData.writeCount = 1;
	readData.readBuf = rxBuffer;
	readData.readCount = 2;
	I2C_transfer(*pI2C, &readData);
}

static void TMP007_AddData(char *buf)
{
	//System_printf("TMP007: AddData index %i tick %i\n", tmp007_numData, Clock_getTicks() * Clock_tickPeriod / 1000);
	System_flush();
	if (tmp007_numData >= TMP007_NUM_VALUES) {
		System_printf("TMP007 raw data buffer is full, waiting for conversion.\n");
	} else {
		rawData[tmp007_numData] = (buf[0] << 8) | buf[1];
		++tmp007_numData;
	}
}

void TMP007_HandleMsg(I2C_Transaction *msg, Bool transfer)
{
	uint8_t reg = *(uint8_t*)msg->writeBuf;
	switch (reg) {
	case TMP007_REG_TEMP:
		TMP007_AddData(msg->readBuf);
		//Event_post(globalEvents, TMP007_READ_COMPLETE);
		break;
	default:
		break;
	}
}

static float TMP007_ConvertTemperature(uint16_t temperatureRaw)
{
	float temperature = 0.f;
	int i, isNegative;
	isNegative = temperatureRaw & 0x8000;

	if (temperatureRaw & 1) /*check data invalid bit*/
		return 0.0f;

	if (isNegative) {
		temperatureRaw = ~(temperatureRaw) + 1; /*two's complement*/
	}

	for (i = 2; i < 15; ++i) {
		int isBitSet = (temperatureRaw >> i) & 1;
		int exp = i - 7;
		if (isBitSet) {
			temperature += pow2(exp);
		}
	}

	if (isNegative)
		temperature *= -1;

	return temperature;
}

void TMP007_ConvertData()
{
	int i;

	//System_printf("TMP007: Starting conversion, index: (%i/%i)\n", tmp007_numData, TMP007_NUM_VALUES);
	for (i = 0; i < tmp007_numData; ++i) {
		TMP007_data[i] = TMP007_ConvertTemperature(rawData[i]);
	}
	//tmp007_numData = 0;
	//System_printf("TMP007 conversion complete.\n");
}

/*
float TMP007_GetTemperature()
{
	float temperature = 0.0f;
	uint16_t temperatureRaw = 0;
	I2C_Transaction i2cTransaction;
	char txBuffer[4];
	char rxBuffer[2];

	txBuffer[0] = TMP007_REG_TEMP;
    i2cTransaction.slaveAddress = Board_TMP007_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;

	if (I2C_transfer(*pI2C, &i2cTransaction)) {
		int i, isNegative;
		temperatureRaw = (rxBuffer[0] << 8) | rxBuffer[1];
		//temperatureRaw = 0b1100000001000000; //-127.5
		isNegative = temperatureRaw & 0x8000;

		if (temperatureRaw & 1)
			return 0.0f;

		if (isNegative) {
			temperatureRaw = ~(temperatureRaw) + 1;
		}

		for (i = 2; i < 15; ++i) {
			int isBitSet = (temperatureRaw >> i) & 1;
			int exp = i - 7;
			if (isBitSet) {
				temperature += pow2(exp);
			}
		}

		if (isNegative)
			temperature *= -1;
	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}
	return temperature;
}
*/

