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

#include <stdio.h>
#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

#include "util/math.h"
#include "Board.h"
#include "../eventdefs.h"
#include "sensordefs.h"

#define TMP007_REG_TEMP	0x03

extern I2C_Handle *pI2C;
extern Event_Handle g_hEvent;

float TMP007_data[TMP007_NUM_VALUES];
extern int TMP007_numData;

void TMP007_Setup();
void TMP007_HandleMsg(I2C_Transaction *msg, Bool transfer);
void TMP007_Read();
//float TMP007_ConvertTemp(uint16_t temperatureRaw);
void TMP007_ConvertData();
//float TMP007_GetTemperature();

#endif /* TMP007_H_ */
