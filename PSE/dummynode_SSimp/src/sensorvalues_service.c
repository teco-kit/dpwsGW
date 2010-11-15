/*
 * sensorvalues_service.c
 *
 *  Created on: 02.03.2010
 *      Author: riedel
 */
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sens_types.h>
#include "sensorvalues_service.h"

static struct timeval timeoffset,uptime;
static sens_SensorConfigurationType sensorConfig={};
static sens_SensorConfigurationType allSensorConfig={};
static int time_set=0;

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))


void get_time(struct timeval* rel,struct timeval* abs)
{
	abs->tv_sec=rel->tv_sec-timeoffset.tv_sec;
	abs->tv_usec=rel->tv_usec-timeoffset.tv_usec;
}

void set_curtime(struct timeval* abs)
{
	gettimeofday(&timeoffset,NULL);
	timeoffset.tv_sec-=abs->tv_sec;
	timeoffset.tv_usec-=abs->tv_usec;
}

void init_sensors()
{
	/* radomize local time for dummy*/
	gettimeofday(&timeoffset,NULL);
	gettimeofday(&uptime,NULL);

	/* initial sensors with defaults*/
	sens_SensorConfigurationType_add_acceleration(&sensorConfig)->rate=6;
	sens_SensorConfigurationType_add_acceleration(&allSensorConfig)->rate=1;
	sens_SensorConfigurationType_add_temperature(&sensorConfig)->rate=7;
	sens_SensorConfigurationType_add_temperature(&allSensorConfig)->rate=4;
	sens_SensorConfigurationType_add_force(&allSensorConfig)->rate=1;
	sens_SensorConfigurationType_add_ambientLight(&allSensorConfig)->rate=1;
}

int get_sensor_values(sens_SSimpSample *s, int16_t eventCount) /* gets called for event and get */
{
	int has_data=0;

	sens_DateTime *t = sens_SSimpSample_add_timeStamp(s);

	  gettimeofday(t,NULL);


    if(	    (eventCount<0 && allSensorConfig.ambientLight.len)	||
    		(sensorConfig.ambientLight.len && 0==(eventCount%(1<<sensorConfig.ambientLight.element[0].rate))
    				))
    {
        sens_TSL2550Sample *light = sens_SSimpSample_add_light(s);
        light->daylight=100+rand()*10;
        light->infrared=100-light->daylight;
        has_data=1;
    }

	if( (eventCount<0 && allSensorConfig.force.len)	||(sensorConfig.force.len && 0==(eventCount%(1<<sensorConfig.force.element[0].rate))))
	{
	  sens_SSimpSample_add_force(s)->value = rand()*100;
	  has_data=1;
	}

	if( (eventCount<0 && allSensorConfig.temperature.len)	||(sensorConfig.temperature.len && 0==(eventCount%(1<<sensorConfig.temperature.element[0].rate))))
	{
		sens_SSimpSample_add_temperature(s)->value=23;
		has_data=1;
	}

    if( (eventCount<0 && allSensorConfig.acceleration.len)	|| (sensorConfig.acceleration.len && 0==(eventCount%(1<<sensorConfig.acceleration.element[0].rate))))
	{
		int period=10000; //in us
		double x=((double)(t->tv_usec%period))/period*2*M_PI;
		sens_ADXL210Sample *accl = sens_SSimpSample_add_accelleration(s);

		accl->x = sin(x)*1000;
		accl->y = sin(x+M_PI_2)*1000;
		accl->z = sin(x+M_PI)*1000;
		has_data=1;
	}

    if(time_set)
    {
    	get_time(t,t);
    }
    else
    {
    	s->timeStamp.len=0;
    }
    return (!has_data);
}

#define config_rate(SENSOR,s)\
do{sens_SSimpRateConfig *c,*c_a;\
if(NULL!=(c=sens_SensorConfigurationType_next_##SENSOR(s,NULL))&&NULL!=(c_a=sens_SensorConfigurationType_next_##SENSOR(&allSensorConfig,NULL)))\
		 sens_SensorConfigurationType_add_##SENSOR(&sensorConfig)->rate=MAX(c->rate,c_a->rate);\
}while(0)

int configure_sensors(sens_SSimpControl *in, sens_SSimpStatus *out)
{

	gettimeofday(&(out->currentTime),NULL);

	get_time(&out->currentTime,&out->currentTime);
	get_time(&out->upTime,&uptime);

	/*set new time*/
	if(in->newTime.len)
	{
	  set_curtime(in->newTime.element);
	  time_set=1;
	}

	/*set new sensor config*/
	if(in->sensorConfig.len)
	{
		config_rate(acceleration,in->sensorConfig.element);
		config_rate(ambientLight,in->sensorConfig.element);
		config_rate(audio,in->sensorConfig.element);
		config_rate(force,in->sensorConfig.element);
		config_rate(temperature,in->sensorConfig.element);
	}

	/*return current/new values*/
	memcpy(&out->sensorConfig,&sensorConfig,sizeof(sens_SensorConfigurationType));
	memcpy(&out->allSensorConfig,&allSensorConfig,sizeof(sens_SensorConfigurationType));

	out->batteryVoltage=1200;

	return 0;
}
