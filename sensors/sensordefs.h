/*
 * sensordefs.h
 *
 *  Created on: Nov 2, 2016
 *      Author: Valtteri
 */

#ifndef SENSORS_SENSORDEFS_H_
#define SENSORS_SENSORDEFS_H_

#define CONVERSION_RATE_MS 5000
#define TMP007_READ_RATE_MS 500
#define BMP280_READ_RATE_MS 1000
#define OPT3001_READ_RATE_MS 1000

#define TMP007_NUM_VALUES (CONVERSION_RATE_MS / TMP007_READ_RATE_MS)
#define BMP280_NUM_VALUES (CONVERSION_RATE_MS / BMP280_READ_RATE_MS)
#define OPT3001_NUM_VALUES (CONVERSION_RATE_MS / OPT3001_READ_RATE_MS)

#endif /* SENSORS_SENSORDEFS_H_ */
