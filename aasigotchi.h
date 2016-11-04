/*
 * aasigotchi.h
 *
 *  Created on: Nov 4, 2016
 *      Author: tsei
 */

#ifndef AASIGOTCHI_H_
#define AASIGOTCHI_H_

#include <stdint.h>

typedef struct {
	char name[17];
	const uint8_t image[8];
	uint32_t l;
	uint32_t a;
	uint32_t r;
	uint32_t s;
} aasigotchi_data;


#endif /* AASIGOTCHI_H_ */
