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

static char txBuffer[5];
static char rxBuffer[24];

static I2C_Transaction readData = {
	.slaveAddress = Board_BMP280_ADDR,
	.writeBuf = txBuffer,
	.writeCount = 1,
	.readBuf = rxBuffer,
	.readCount = 6
};
static I2C_Transaction writeConfig;
static I2C_Transaction writeMeas;
static I2C_Transaction writeTrimming;


static uint32_t rawData_Pres[BMP280_NUM_VALUES];
static uint32_t rawData_Temp[BMP280_NUM_VALUES];
float BMP280_presData[BMP280_NUM_VALUES];
//int BMP280_numData;

void BMP280_SetTrimming(char *v)
{
	dig_T1 = (v[1] << 8) | v[0];
	dig_T2 = (v[3] << 8) | v[2];
	dig_T3 = (v[5] << 8) | v[4];
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

void BMP280_Setup(I2C_Handle *i2c)
{
	writeConfig.slaveAddress = Board_BMP280_ADDR;
	txBuffer[0] = BMP280_REG_CONFIG;
	txBuffer[1] = 0x40;
	writeConfig.writeBuf = txBuffer;
	writeConfig.writeCount = 2;
	writeConfig.readBuf = NULL;
	writeConfig.readCount = 0;

	writeMeas.slaveAddress = Board_BMP280_ADDR;
	txBuffer[2] = BMP280_REG_CTRL_MEAS;
	txBuffer[3] = 0x2F;
	writeMeas.writeBuf = &txBuffer[2];
	writeMeas.writeCount = 2;
	writeMeas.readBuf = NULL;
	writeMeas.readCount = 0;

	writeTrimming.slaveAddress = Board_BMP280_ADDR;
	txBuffer[4] = BMP280_REG_T1;
	writeTrimming.writeBuf = &txBuffer[4];
	writeTrimming.writeCount = 1;
	writeTrimming.readBuf = rxBuffer;
	writeTrimming.readCount = 24;

	I2C_transfer(*i2c, &writeConfig);
	I2C_transfer(*i2c, &writeMeas);
	I2C_transfer(*i2c, &writeTrimming);
}

static void BMP280_AddData(char *buf)
{
	if (BMP280_numData >= BMP280_NUM_VALUES) {
		System_printf("BMP280 raw data buffers are full, waiting for conversion.\n");
	} else {
		rawData_Pres[BMP280_numData] = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
		rawData_Temp[BMP280_numData] = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
		++BMP280_numData;
	}
	System_flush();
}

void BMP280_Read()
{
	txBuffer[0] = BMP280_REG_PRES;
	I2C_transfer(*pI2C, &readData);
}

void BMP280_HandleMsg(I2C_Transaction *msg, Bool transfer)
{
	uint8_t reg = *(uint8_t*)msg->writeBuf;
	switch (reg) {
	case BMP280_REG_CONFIG:
		if (transfer)
			System_printf("BMP280: Config write ok!\n");
		else
			System_abort("BMP280: Config write failed!\n");
		break;
	case BMP280_REG_CTRL_MEAS:
		if (transfer)
			System_printf("BMP280: Ctrl meas write ok!\n");
		else
			System_abort("BMP280: Ctrl meas write failed!\n");
		break;
	case BMP280_REG_T1:
		if (transfer) {
			System_printf("BMP280: Trimming write ok!\n");
			BMP280_SetTrimming(msg->readBuf);
			Event_post(globalEvents, SENSOR_SETUP_COMPLETE);
		} else
			System_abort("BMP280: Trimming write failed!\n");
		break;
	case BMP280_REG_PRES:
		BMP280_AddData(msg->readBuf);
		break;
	default:
		break;
	}
}

void BMP280_ConvertData()
{
	int i;
	for (i = 0; i < BMP280_numData; ++i) {
		BMP280_ConvertTemperature(rawData_Temp[i]);
		BMP280_presData[i] = BMP280_ConvertPressure(rawData_Pres[i]);
	}
}
