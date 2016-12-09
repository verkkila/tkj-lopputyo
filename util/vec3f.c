/*
 * vec3f.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include "vec3f.h"

float vec3f_GetLength(const vec3f * const this)
{
	return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}
