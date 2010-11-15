#ifndef ___SENS_TYPES_H
#define ___SENS_TYPES_H 1
#include <stdint.h>
#include <sys/time.h>

#define sens_UnsignedInt uint32_t
#define sens_SignedInt int32_t
#define sens_Int int32_t

#define sens_UnsignedShort uint16_t
#define sens_SignedShort int16_t
#define sens_Short int16_t

#define sens_UnsignedByte uint8_t
#define sens_SignedByte int8_t
#define sens_Byte int8_t

#define sens_DateTime struct timeval

typedef sens_Short sens_XType;

typedef sens_Byte sens_ValueType;

typedef sens_Short sens_YType;

typedef sens_Short sens_ZType;

struct sens_ADXL210Sample {

	sens_XType x;

	sens_YType y;

	sens_ZType z;

	struct {
		size_t len;
		sens_Byte element[1];
	} index;

};

static sens_Byte *sens_ADXL210Sample_add_index(struct sens_ADXL210Sample *self) __attribute__((__unused__));
static sens_Byte *sens_ADXL210Sample_add_index(struct sens_ADXL210Sample *self) {
	if (self->index.len < 1)
		return &(self->index.element[(self->index.len++)]);
	else
		return NULL;
}

static sens_Byte *sens_ADXL210Sample_next_index(
		struct sens_ADXL210Sample *self, sens_Byte *last) __attribute__((__unused__));
static sens_Byte *sens_ADXL210Sample_next_index(
		struct sens_ADXL210Sample *self, sens_Byte *last) {
	if (self->index.len == 0)
		return NULL;
	if (last == NULL)
		return self->index.element;
	if (last != &(self->index.element[self->index.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_SP101Sample {

	sens_UnsignedByte volume;

	struct {
		size_t len;
		sens_Byte element[1];
	} index;

};

static sens_Byte *sens_SP101Sample_add_index(struct sens_SP101Sample *self) __attribute__((__unused__));
static sens_Byte *sens_SP101Sample_add_index(struct sens_SP101Sample *self) {
	if (self->index.len < 1)
		return &(self->index.element[(self->index.len++)]);
	else
		return NULL;
}

static sens_Byte *sens_SP101Sample_next_index(struct sens_SP101Sample *self,
		sens_Byte *last) __attribute__((__unused__));
static sens_Byte *sens_SP101Sample_next_index(struct sens_SP101Sample *self,
		sens_Byte *last) {
	if (self->index.len == 0)
		return NULL;
	if (last == NULL)
		return self->index.element;
	if (last != &(self->index.element[self->index.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_TSL2550Sample {

	sens_UnsignedShort daylight;

	sens_UnsignedShort infrared;

	struct {
		size_t len;
		sens_Byte element[1];
	} index;

};

static sens_Byte *sens_TSL2550Sample_add_index(struct sens_TSL2550Sample *self) __attribute__((__unused__));
static sens_Byte *sens_TSL2550Sample_add_index(struct sens_TSL2550Sample *self) {
	if (self->index.len < 1)
		return &(self->index.element[(self->index.len++)]);
	else
		return NULL;
}

static sens_Byte *sens_TSL2550Sample_next_index(
		struct sens_TSL2550Sample *self, sens_Byte *last) __attribute__((__unused__));
static sens_Byte *sens_TSL2550Sample_next_index(
		struct sens_TSL2550Sample *self, sens_Byte *last) {
	if (self->index.len == 0)
		return NULL;
	if (last == NULL)
		return self->index.element;
	if (last != &(self->index.element[self->index.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_FSR152Sample {

	sens_UnsignedByte value;

	struct {
		size_t len;
		sens_Byte element[1];
	} index;

};

static sens_Byte *sens_FSR152Sample_add_index(struct sens_FSR152Sample *self) __attribute__((__unused__));
static sens_Byte *sens_FSR152Sample_add_index(struct sens_FSR152Sample *self) {
	if (self->index.len < 1)
		return &(self->index.element[(self->index.len++)]);
	else
		return NULL;
}

static sens_Byte *sens_FSR152Sample_next_index(struct sens_FSR152Sample *self,
		sens_Byte *last) __attribute__((__unused__));
static sens_Byte *sens_FSR152Sample_next_index(struct sens_FSR152Sample *self,
		sens_Byte *last) {
	if (self->index.len == 0)
		return NULL;
	if (last == NULL)
		return self->index.element;
	if (last != &(self->index.element[self->index.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_TC74Sample {

	sens_ValueType value;

	struct {
		size_t len;
		sens_Byte element[1];
	} index;

};

static sens_Byte *sens_TC74Sample_add_index(struct sens_TC74Sample *self) __attribute__((__unused__));
static sens_Byte *sens_TC74Sample_add_index(struct sens_TC74Sample *self) {
	if (self->index.len < 1)
		return &(self->index.element[(self->index.len++)]);
	else
		return NULL;
}

static sens_Byte *sens_TC74Sample_next_index(struct sens_TC74Sample *self,
		sens_Byte *last) __attribute__((__unused__));
static sens_Byte *sens_TC74Sample_next_index(struct sens_TC74Sample *self,
		sens_Byte *last) {
	if (self->index.len == 0)
		return NULL;
	if (last == NULL)
		return self->index.element;
	if (last != &(self->index.element[self->index.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_SSimpSample {

	struct {
		size_t len;
		struct sens_ADXL210Sample element[1];
	} accelleration;

	struct {
		size_t len;
		struct sens_SP101Sample element[1];
	} audio;

	struct {
		size_t len;
		struct sens_TSL2550Sample element[1];
	} light;

	struct {
		size_t len;
		struct sens_FSR152Sample element[1];
	} force;

	struct {
		size_t len;
		struct sens_TC74Sample element[1];
	} temperature;

	struct {
		size_t len;
		sens_DateTime element[1];
	} timeStamp;

};

static struct sens_ADXL210Sample *sens_SSimpSample_add_accelleration(
		struct sens_SSimpSample *self) __attribute__((__unused__));
static struct sens_ADXL210Sample *sens_SSimpSample_add_accelleration(
		struct sens_SSimpSample *self) {
	if (self->accelleration.len < 1)
		return &(self->accelleration.element[(self->accelleration.len++)]);
	else
		return NULL;
}

static struct sens_ADXL210Sample *sens_SSimpSample_next_accelleration(
		struct sens_SSimpSample *self, struct sens_ADXL210Sample *last) __attribute__((__unused__));
static struct sens_ADXL210Sample *sens_SSimpSample_next_accelleration(
		struct sens_SSimpSample *self, struct sens_ADXL210Sample *last) {
	if (self->accelleration.len == 0)
		return NULL;
	if (last == NULL)
		return self->accelleration.element;
	if (last != &(self->accelleration.element[self->accelleration.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SP101Sample *sens_SSimpSample_add_audio(
		struct sens_SSimpSample *self) __attribute__((__unused__));
static struct sens_SP101Sample *sens_SSimpSample_add_audio(
		struct sens_SSimpSample *self) {
	if (self->audio.len < 1)
		return &(self->audio.element[(self->audio.len++)]);
	else
		return NULL;
}

static struct sens_SP101Sample *sens_SSimpSample_next_audio(
		struct sens_SSimpSample *self, struct sens_SP101Sample *last) __attribute__((__unused__));
static struct sens_SP101Sample *sens_SSimpSample_next_audio(
		struct sens_SSimpSample *self, struct sens_SP101Sample *last) {
	if (self->audio.len == 0)
		return NULL;
	if (last == NULL)
		return self->audio.element;
	if (last != &(self->audio.element[self->audio.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_TSL2550Sample *sens_SSimpSample_add_light(
		struct sens_SSimpSample *self) __attribute__((__unused__));
static struct sens_TSL2550Sample *sens_SSimpSample_add_light(
		struct sens_SSimpSample *self) {
	if (self->light.len < 1)
		return &(self->light.element[(self->light.len++)]);
	else
		return NULL;
}

static struct sens_TSL2550Sample *sens_SSimpSample_next_light(
		struct sens_SSimpSample *self, struct sens_TSL2550Sample *last) __attribute__((__unused__));
static struct sens_TSL2550Sample *sens_SSimpSample_next_light(
		struct sens_SSimpSample *self, struct sens_TSL2550Sample *last) {
	if (self->light.len == 0)
		return NULL;
	if (last == NULL)
		return self->light.element;
	if (last != &(self->light.element[self->light.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_FSR152Sample *sens_SSimpSample_add_force(
		struct sens_SSimpSample *self) __attribute__((__unused__));
static struct sens_FSR152Sample *sens_SSimpSample_add_force(
		struct sens_SSimpSample *self) {
	if (self->force.len < 1)
		return &(self->force.element[(self->force.len++)]);
	else
		return NULL;
}

static struct sens_FSR152Sample *sens_SSimpSample_next_force(
		struct sens_SSimpSample *self, struct sens_FSR152Sample *last) __attribute__((__unused__));
static struct sens_FSR152Sample *sens_SSimpSample_next_force(
		struct sens_SSimpSample *self, struct sens_FSR152Sample *last) {
	if (self->force.len == 0)
		return NULL;
	if (last == NULL)
		return self->force.element;
	if (last != &(self->force.element[self->force.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_TC74Sample *sens_SSimpSample_add_temperature(
		struct sens_SSimpSample *self) __attribute__((__unused__));
static struct sens_TC74Sample *sens_SSimpSample_add_temperature(
		struct sens_SSimpSample *self) {
	if (self->temperature.len < 1)
		return &(self->temperature.element[(self->temperature.len++)]);
	else
		return NULL;
}

static struct sens_TC74Sample *sens_SSimpSample_next_temperature(
		struct sens_SSimpSample *self, struct sens_TC74Sample *last) __attribute__((__unused__));
static struct sens_TC74Sample *sens_SSimpSample_next_temperature(
		struct sens_SSimpSample *self, struct sens_TC74Sample *last) {
	if (self->temperature.len == 0)
		return NULL;
	if (last == NULL)
		return self->temperature.element;
	if (last != &(self->temperature.element[self->temperature.len - 1]))
		return last++;
	else
		return NULL;
}

static sens_DateTime *sens_SSimpSample_add_timeStamp(struct sens_SSimpSample *self) __attribute__((__unused__));
static sens_DateTime *sens_SSimpSample_add_timeStamp(struct sens_SSimpSample *self) {if(self->timeStamp.len<1) return &(self->timeStamp.element[(self->timeStamp.len++)]); else return NULL;}

static sens_DateTime *sens_SSimpSample_next_timeStamp(struct sens_SSimpSample *self, sens_DateTime *last) __attribute__((__unused__));
static sens_DateTime *sens_SSimpSample_next_timeStamp(struct sens_SSimpSample *self, sens_DateTime *last) {
	if(self->timeStamp.len==0) return NULL;
	if(last==NULL) return self->timeStamp.element;
	if(last!=&(self->timeStamp.element[self->timeStamp.len-1])) return last++;
	else return NULL;}

struct sens_SSimpRateConfig {

	sens_Byte rate;

};

struct sens_SensorConfigurationType {

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} acceleration;

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} audio;

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} light;

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} ambientLight;

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} force;

	struct {
		size_t len;
		struct sens_SSimpRateConfig element[1];
	} temperature;

};

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_add_acceleration(
				struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_acceleration(
		struct sens_SensorConfigurationType *self) {
	if (self->acceleration.len < 1)
		return &(self->acceleration.element[(self->acceleration.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_next_acceleration(
				struct sens_SensorConfigurationType *self,
				struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_acceleration(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->acceleration.len == 0)
		return NULL;
	if (last == NULL)
		return self->acceleration.element;
	if (last != &(self->acceleration.element[self->acceleration.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_audio(
		struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_audio(
		struct sens_SensorConfigurationType *self) {
	if (self->audio.len < 1)
		return &(self->audio.element[(self->audio.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_audio(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_audio(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->audio.len == 0)
		return NULL;
	if (last == NULL)
		return self->audio.element;
	if (last != &(self->audio.element[self->audio.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_light(
		struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_light(
		struct sens_SensorConfigurationType *self) {
	if (self->light.len < 1)
		return &(self->light.element[(self->light.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_light(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_light(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->light.len == 0)
		return NULL;
	if (last == NULL)
		return self->light.element;
	if (last != &(self->light.element[self->light.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_add_ambientLight(
				struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_ambientLight(
		struct sens_SensorConfigurationType *self) {
	if (self->ambientLight.len < 1)
		return &(self->ambientLight.element[(self->ambientLight.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_next_ambientLight(
				struct sens_SensorConfigurationType *self,
				struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_ambientLight(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->ambientLight.len == 0)
		return NULL;
	if (last == NULL)
		return self->ambientLight.element;
	if (last != &(self->ambientLight.element[self->ambientLight.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_force(
		struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_force(
		struct sens_SensorConfigurationType *self) {
	if (self->force.len < 1)
		return &(self->force.element[(self->force.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_force(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_force(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->force.len == 0)
		return NULL;
	if (last == NULL)
		return self->force.element;
	if (last != &(self->force.element[self->force.len - 1]))
		return last++;
	else
		return NULL;
}

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_add_temperature(
				struct sens_SensorConfigurationType *self) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_add_temperature(
		struct sens_SensorConfigurationType *self) {
	if (self->temperature.len < 1)
		return &(self->temperature.element[(self->temperature.len++)]);
	else
		return NULL;
}

static struct sens_SSimpRateConfig
		*sens_SensorConfigurationType_next_temperature(
				struct sens_SensorConfigurationType *self,
				struct sens_SSimpRateConfig *last) __attribute__((__unused__));
static struct sens_SSimpRateConfig *sens_SensorConfigurationType_next_temperature(
		struct sens_SensorConfigurationType *self,
		struct sens_SSimpRateConfig *last) {
	if (self->temperature.len == 0)
		return NULL;
	if (last == NULL)
		return self->temperature.element;
	if (last != &(self->temperature.element[self->temperature.len - 1]))
		return last++;
	else
		return NULL;
}

struct sens_SSimpStatus {

	struct sens_SensorConfigurationType sensorConfig;

	struct sens_SensorConfigurationType allSensorConfig;

	sens_UnsignedShort batteryVoltage;

	sens_DateTime currentTime;

	sens_DateTime upTime;

};

struct sens_SSimpControl {

	struct {
		size_t len;
		struct sens_SensorConfigurationType element[1];
	} sensorConfig;

	struct {
		size_t len;
		sens_DateTime element[1];
	} newTime;

};

static struct sens_SensorConfigurationType *sens_SSimpControl_add_sensorConfig(
		struct sens_SSimpControl *self) __attribute__((__unused__));
static struct sens_SensorConfigurationType *sens_SSimpControl_add_sensorConfig(
		struct sens_SSimpControl *self) {
	if (self->sensorConfig.len < 1)
		return &(self->sensorConfig.element[(self->sensorConfig.len++)]);
	else
		return NULL;
}

static struct sens_SensorConfigurationType
		*sens_SSimpControl_next_sensorConfig(struct sens_SSimpControl *self,
				struct sens_SensorConfigurationType *last) __attribute__((__unused__));
static struct sens_SensorConfigurationType *sens_SSimpControl_next_sensorConfig(
		struct sens_SSimpControl *self,
		struct sens_SensorConfigurationType *last) {
	if (self->sensorConfig.len == 0)
		return NULL;
	if (last == NULL)
		return self->sensorConfig.element;
	if (last != &(self->sensorConfig.element[self->sensorConfig.len - 1]))
		return last++;
	else
		return NULL;
}

static sens_DateTime *sens_SSimpControl_add_newTime(struct sens_SSimpControl *self) __attribute__((__unused__));
static sens_DateTime *sens_SSimpControl_add_newTime(struct sens_SSimpControl *self) {if(self->newTime.len<1) return &(self->newTime.element[(self->newTime.len++)]); else return NULL;}

static sens_DateTime *sens_SSimpControl_next_newTime(struct sens_SSimpControl *self, sens_DateTime *last) __attribute__((__unused__));
static sens_DateTime *sens_SSimpControl_next_newTime(struct sens_SSimpControl *self, sens_DateTime *last) {
	if(self->newTime.len==0) return NULL;
	if(last==NULL) return self->newTime.element;
	if(last!=&(self->newTime.element[self->newTime.len-1])) return last++;
	else return NULL;}

typedef struct sens_SP101Sample sens_SP101Sample;

typedef struct sens_TC74Sample sens_TC74Sample;

typedef struct sens_SSimpStatus sens_SSimpStatus;

typedef struct sens_ADXL210Sample sens_ADXL210Sample;

typedef struct sens_SSimpRateConfig sens_SSimpRateConfig;

typedef struct sens_SSimpControl sens_SSimpControl;

typedef struct sens_SSimpSample sens_SSimpSample;

typedef struct sens_FSR152Sample sens_FSR152Sample;

typedef struct sens_TSL2550Sample sens_TSL2550Sample;

typedef struct sens_SensorConfigurationType sens_SensorConfigurationType;

#endif
