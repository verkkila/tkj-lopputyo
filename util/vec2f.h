/*
 * vec2f.h
 *
 *  Created on: Nov 8, 2016
 *      Author: Valtteri
 */

#ifndef UTIL_VEC2F_H_
#define UTIL_VEC2F_H_

#include <math.h>

typedef struct {
	float x, y;
} vec2f;

float vec2f_Length(const vec2f * const vec);
vec2f vec2f_Mult(const vec2f * const vec, float scale);

#endif /* UTIL_VEC2F_H_ */
