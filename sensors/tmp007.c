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
	if (TMP007_numData >= TMP007_NUM_VALUES) {
		System_printf("TMP007 raw data buffer is full, waiting for conversion.\n");
	} else {
		rawData[TMP007_numData] = (buf[0] << 8) | buf[1];
		++TMP007_numData;
	}
	System_flush();
}

void TMP007_HandleMsg(I2C_Transaction *msg, Bool transfer)
{
	uint8_t reg = *(uint8_t*)msg->writeBuf;
	switch (reg) {
	case TMP007_REG_TEMP:
		TMP007_AddData(msg->readBuf);
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

	for (i = 0; i < TMP007_numData; ++i) {
		TMP007_data[i] = TMP007_ConvertTemperature(rawData[i]);
	}
}

