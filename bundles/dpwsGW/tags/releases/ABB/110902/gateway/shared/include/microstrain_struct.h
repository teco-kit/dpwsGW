/*
 * microstrain_struct.h
 *
 *  Created on: 26.08.2011
 *      Author: Daniel Wildschut
 */

#ifndef MICROSTRAIN_STRUCT_H_
#define MICROSTRAIN_STRUCT_H_

typedef struct {
	char rate[64];
	char duration[64];
} logging_info;

typedef struct {
	float values[8];
} logging_sample;

typedef struct {
	unsigned char channelmask;
	unsigned char channels;
	unsigned char samplecount;
	float rate;
	int prevsamplecount;
	logging_sample samples[132];
} logging_page;

typedef struct {
	int count;
} session_info;

typedef struct {
	float values[8];
	float timertick;
	float delta;
	unsigned char channelmask;
} ldc_message;

typedef struct {
	char rate[64];
	char duration[64];
} LDC_info;

typedef struct {
	char status[256];
} device_info;

#endif /* MICROSTRAIN_STRUCT_H_ */
