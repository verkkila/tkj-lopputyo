/*
 * mpu9250.h
 *
 *  Adopted for SensorTag from https://github.com/kriswiner/MPU-9250 by Kris Winer
 *
 *  Created on: 8.10.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 * 	Datasheet: https://store.invensense.com/datasheets/invensense/MPU9250REV1.0.pdf
 */

#ifndef MPU9250_H_
#define MPU9250_H_

#include <stdio.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/I2C.h>
#include "util/vec3f.h"
#include "sensordefs.h"

extern I2C_Handle *pMpuI2C;

extern int mpu9250_index;
vec3f MPU9250_Data[MPU9250_NUM_VALUES];

void MPU9250_Setup(I2C_Handle *i2c_orig);
void MPU9250_Read();
void MPU9250_TransferComplete(I2C_Transaction *msg);
void MPU9250_AddData(vec3f *a);
void MPU9250_GetData(vec3f *accel, vec3f *gyro);

#endif /* MPU9250_H_ */
