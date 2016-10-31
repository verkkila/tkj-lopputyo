/*
 * tmp007.c
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */
#include "tmp007.h"

void TMP007_Setup(I2C_Handle *i2c)
{
	System_printf("TMP007: Config not needed!\n");
    System_flush();
}

float TMP007_GetTemperature(I2C_Handle *i2c)
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

	if (I2C_transfer(*i2c, &i2cTransaction)) {
		int i, isNegative;
		temperatureRaw = (rxBuffer[0] << 8) | rxBuffer[1];
		//temperatureRaw = 0b1100000001000000; //-127.5
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

		System_flush();
	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}
	return temperature;
}


