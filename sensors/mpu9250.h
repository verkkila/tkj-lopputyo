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

#include <ti/drivers/I2C.h>
#include "util/vec3f.h"

void MPU9250_Setup(I2C_Handle *i2c_orig);
void MPU9250_GetData(I2C_Handle *i2c, vec3f *accel, vec3f *gyro);

#endif /* MPU9250_H_ */
