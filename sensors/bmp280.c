/*
 * bmp280.c
 *
 *  Created on: 7.10.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 * 	Datasheet: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001-12.pdf
 */

#include <xdc/runtime/System.h>
#include <stdio.h>
#include "Board.h"
#include "bmp280.h"

// conversion constants
uint16_t dig_T1;
int16_t  dig_T2;
int16_t  dig_T3;
uint16_t dig_P1;
int16_t  dig_P2;
int16_t  dig_P3;
int16_t  dig_P4;
int16_t  dig_P5;
int16_t  dig_P6;
int16_t  dig_P7;
int16_t  dig_P8;
int16_t  dig_P9;
int32_t	 t_fine = 0;

// i2c
I2C_Transaction i2cTransaction;
char txBuffer[4];
char rxBuffer[24];

static void BMP280_SetTrimming(char *v) {

	dig_T1 = (v[1] << 8) | v[0];
	dig_T2 = (v[3] << 8) | v[2];
	dig_T3 = (v[5] << 8) | v[4];
	dig_P1 = (v[7] << 8) | v[6];
	dig_P2 = (v[9] << 8) | v[8];
	dig_P3 = (v[11] << 8) | v[10];
	dig_P1 = (v[7] << 8) | v[6];
	dig_P2 = (v[9] << 8) | v[8];
	dig_P3 = (v[11] << 8) | v[10];
	dig_P4 = (v[13] << 8) | v[12];
	dig_P5 = (v[15] << 8) | v[14];
	dig_P6 = (v[17] << 8) | v[16];
	dig_P7 = (v[19] << 8) | v[18];
	dig_P8 = (v[21] << 8) | v[20];
	dig_P9 = (v[23] << 8) | v[22];
}

static double BMP280_ConvertTemperature(uint32_t adc_T) {

	double ret = 0.0;
	int32_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)dig_T1 <<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;

	ret = (double)((t_fine * 5 + 128) >> 8);
	ret /= 100.0;
	return ret;
}

static double BMP280_ConvertPressure(uint32_t adc_P) {

	double ret = 0.0;
	int64_t var1, var2, p;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0) {
	    return 0.0;  // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p<<31) - var2)*3125) / var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;

	ret = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	ret /= 256.0;
	return ret;
}

void BMP280_Setup(I2C_Handle *i2c) {

    i2cTransaction.slaveAddress = Board_BMP280_ADDR;
    txBuffer[0] = BMP280_REG_CONFIG;
    txBuffer[1] = 0x40;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {
        System_printf("BMP280: Config write ok\n");
    } else {
        System_printf("BMP280: Config write failed!\n");
    }
    System_flush();

    i2cTransaction.slaveAddress = Board_BMP280_ADDR;
    txBuffer[0] = BMP280_REG_CTRL_MEAS;
    txBuffer[1] = 0x2F;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {
        System_printf("BMP280: Ctrl meas write ok\n");
    } else {
        System_printf("BMP280: Ctrl meas write failed!\n");
    }
    System_flush();

    i2cTransaction.slaveAddress = Board_BMP280_ADDR;
    txBuffer[0] = BMP280_REG_T1;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 24;

    if (I2C_transfer(*i2c, &i2cTransaction)) {
        System_printf("BMP280: Trimming read ok\n");
    } else {
        System_printf("BMP280: Trimming read failed!\n");
    }
    System_flush();

    BMP280_SetTrimming(rxBuffer);
}

float BMP280_GetPressure(I2C_Handle *i2c) {
	uint32_t pressureRaw = 0;
	uint32_t temperatureRaw = 0;
	float pressure = 0.0f;

	txBuffer[0] = BMP280_REG_PRES;
    i2cTransaction.slaveAddress = Board_BMP280_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 6;

    if (I2C_transfer(*i2c, &i2cTransaction)) {
    	pressureRaw = (rxBuffer[0] << 12) | (rxBuffer[1] << 4) | (rxBuffer[2] >> 4);
    	temperatureRaw = (rxBuffer[3] << 12) | (rxBuffer[4] << 4) | (rxBuffer[5] >> 4);
    	BMP280_ConvertTemperature(temperatureRaw);
    	pressure = BMP280_ConvertPressure(pressureRaw);
		// HERE YOU GET THE SENSOR VALUES FROM RXBUFFER
		// ACCORDING TO DATASHEET: GET BOTH TEMPERATURE AND AIR PRESSURE
		// FIRST, USE FUNCTION bmp280_convert_temp() TO CONVERT RAW TEMPERATURE VALUE
    	// THIS WILL INITIALIZE TEMPERATURE COMPENSATION, WHICH IS NEEDED BY PRESSURE CONVERSION
		// THEN, USE FUNCTION bmp280_convert_pres() TO CONVERT RAW PRESSURE VALUE TO PASCALS
    } else {
        System_printf("BMP280: Data read failed!\n");
		System_flush();
    }
    return pressure;
}

