/*
 * opt3001.h
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/opt3001.pdf
 */

#ifndef OPT3001_H_
#define OPT3001_H_

#include <ti/drivers/I2C.h>
#include "util/math.h"

#define OPT3001_REG_RESULT		0x0
#define OPT3001_REG_CONFIG		0x1
#define OPT3001_DATA_READY		0x80

void OPT3001_Setup(I2C_Handle *i2c);
float OPT3001_GetLuminosity(I2C_Handle *i2c);

#endif /* OPT3001_H_ */
