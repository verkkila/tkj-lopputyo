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

#include <string.h>
#include <math.h>

#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Event.h>
#include "util/math.h"
#include "Board.h"
#include "sensordefs.h"
#include "../eventdefs.h"

#define OPT3001_REG_RESULT		0x0
#define OPT3001_REG_CONFIG		0x1
#define OPT3001_DATA_READY		0x80

extern I2C_Handle *pI2C;
extern Event_Handle globalEvents;


float OPT3001_data[OPT3001_NUM_VALUES];
extern int opt3001_numData;

void OPT3001_Setup(I2C_Handle *i2c);
void OPT3001_HandleMsg(I2C_Transaction *msg, Bool transfer);
void OPT3001_Read();
void OPT3001_ConvertData();
float OPT3001_GetLuminosity();

#endif /* OPT3001_H_ */
