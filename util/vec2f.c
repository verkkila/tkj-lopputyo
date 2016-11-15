/*
 * vec2f.c
 *
 *  Created on: Nov 8, 2016
 *      Author: Valtteri
 */

#include "vec2f.h"

float vec2f_Length(const vec2f * const  vec)
{
	return sqrt(vec->x * vec->x + vec->y * vec->y);
}

void vec2f_Normalize(vec2f * const vec)
{
	float length = vec2f_Length(vec);
	vec->x /= length;
	vec->y /= length;
}

vec2f vec2f_Mult(const vec2f * const vec, float scale)
{
	vec2f ret;
	ret.x = vec->x * scale;
	ret.y = vec->y * scale;
	return ret;
}
