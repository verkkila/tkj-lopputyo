/*
 * vec3f.h
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#ifndef UTIL_VEC3F_H_
#define UTIL_VEC3F_H_

typedef struct {
	float x, y, z;
} vec3f;

float vec3f_GetLength(const vec3f * const this);

#endif /* UTIL_VEC3F_H_ */
