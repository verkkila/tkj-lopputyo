/*
 * tmp007.c
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */

#include <xdc/runtime/System.h>
#include <string.h>
#include <math.h>
#include "Board.h"
#include "tmp007.h"

I2C_Transaction i2cTransaction;
char txBuffer[4];
char rxBuffer[2];


void tmp007_setup(I2C_Handle *i2c) {

	System_printf("TMP007: Config not needed!\n");
    System_flush();
}

double tmp007_get_data(I2C_Handle *i2c) {

	double temperature = 0.0;
	int16_t temp_full = 0;

	/* FILL OUT THIS DATA STRUCTURE TO GET TEMPERATURE DATA*/
	txBuffer[0] = TMP007_REG_TEMP;
    i2cTransaction.slaveAddress = Board_TMP007_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;


	if (I2C_transfer(*i2c, &i2cTransaction)) {
		int i = 2;
		temp_full = (rxBuffer[0] << 8) | rxBuffer[1];
		if (temp_full & 0x8000) {
			//negatiivinen
			temp_full = ~(temp_full) + 1;
		}
		for (i = 2; i < 15; ++i) {
			int bit = (temp_full >> i) & 1;
			double exp = i - 7;
			if (bit) {
				temperature += pow(2.0, exp);
			}
		}
		/*
		sprintf(temp, "%f", temperature);
		System_printf("%s\n", temp);
		*/
		System_flush();
		// HERE YOU GET THE TEMPERATURE VALUE FROM RXBUFFER
		// ACCORDING TO DATASHEET

	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}

	return temperature;
}


