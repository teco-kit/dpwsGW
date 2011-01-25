/*
 * sensorvalues_service.h
 *
 *  Created on: 02.03.2010
 *      Author: riedel
 */

#ifndef SENSORVALUES_SERVICE_H_
#define SENSORVALUES_SERVICE_H_
#include <sens_types.h>
int get_sensor_values(sens_SSimpSample *s, int16_t eventCount);
int configure_sensors(sens_SSimpControl *in, sens_SSimpStatus *out);
void init_sensors();
#endif /* SENSORVALUES_SERVICE_H_ */
