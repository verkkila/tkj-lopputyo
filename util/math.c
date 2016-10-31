/*
 * math.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include "math.h"

static const float POW2_NEGATIVE[] = {
	1,			/*1/2^0*/
	0.5f,		/*1/2^1*/
	0.25f,		/*1/2^2*/
	0.125f,		/*1/2^3*/
	0.0625f,	/*1/2^4*/
	0.03125f,	/*1/2^5*/
	0.015625f,	/*1/2^6*/
	0.0078125f,	/*1/2^7*/
	0.00390625f	/*1/2^8*/
};

float pow2(int exponent)
{
	assert(exponent >= -8 && exponent < 32);
	if (exponent < 1)
		return POW2_NEGATIVE[abs(exponent)];
	return 1 << exponent;
}
