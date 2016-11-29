/*
 * sensordefs.h
 *
 *  Created on: Nov 2, 2016
 *      Author: Valtteri
 */

#ifndef SENSORS_SENSORDEFS_H_
#define SENSORS_SENSORDEFS_H_

#define CONVERSION_RATE_MS 3000
#define TMP007_READ_RATE_MS 1000
#define OPT3001_READ_RATE_MS TMP007_READ_RATE_MS
#define BMP280_READ_RATE_MS 1000
#define MPU9250_READ_RATE_MS 1000

#define TMP007_NUM_VALUES (CONVERSION_RATE_MS / TMP007_READ_RATE_MS)
#define BMP280_NUM_VALUES (CONVERSION_RATE_MS / BMP280_READ_RATE_MS)
#define OPT3001_NUM_VALUES (CONVERSION_RATE_MS / OPT3001_READ_RATE_MS)
#define MPU9250_NUM_VALUES (CONVERSION_RATE_MS / MPU9250_READ_RATE_MS)

#define TEMPERATURE_THRESHOLD 15
#define LUMINOSITY_THRESHOLD 200
#define PRESSURE_THRESHOLD 109114
#define MOVEMENT_THRESHOLD 0.5f

#endif /* SENSORS_SENSORDEFS_H_ */
