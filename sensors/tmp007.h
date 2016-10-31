/*
 * tmp007.h
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */

#ifndef TMP007_H_
#define TMP007_H_

#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>
#include "util/math.h"
#include "Board.h"

#define TMP007_REG_TEMP	0x03
#define TMP007_REG_TEMP_DIE 0x03
#define REG_DECIMAL 0x007C
#define REG_WHOLE 0xFF86

void TMP007_Setup(I2C_Handle *i2c);
float TMP007_GetTemperature(I2C_Handle *i2c);

#endif /* TMP007_H_ */
