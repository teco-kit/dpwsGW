
#include "Status_bin2dom.h"
#include <ptr_stack.h>
#include <string.h>
#include <bitsio/read_bits_buf.h>

int Status_bin2dom_run(struct READER_STRUCT *reader, sens_SSimpStatus *dom) {
	int label = 0;
	new_ptr_stack(stack, dom);

	memset(dom, 0, sizeof(*dom));

	for (;;) {
		switch (label) {

// Start State

		case 0: //stateComplexBegin(Status)
		{
			label = 1; //assert 1==label+1

			/* lowerbound=1 upperbound=1*/

			//push(pop()): preinitialized stack

			//push Status: next 

			break;
		} /* stateEnd */

		case 1: //stateComplexBegin(SensorConfig)
		{
			label = 2; //assert 2==label+1

			/* lowerbound=1 upperbound=1*/

			{
				sens_SSimpStatus * cur;
				cur = (sens_SSimpStatus *) top();
				push(&(cur->sensorConfig));
			}

			//push SensorConfig: next 

			break;
		} /* stateEnd */

		case 2: //stateComplexBegin(Acceleration)
		{
			label = 3; //assert 3==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_acceleration(cur);
					push(next);
				}

				//push Acceleration: next 5

			} else {
				label = 5;
				continue;
			} /* Acceleration */

			break;
		} /* stateEnd */

		case 3: //stateBegin(rate)
		{
			label = 4; //assert 4==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 4: //stateComplexEnd(Acceleration) : 0..1
		{

			label = 2; /* constLoopEnd1 /Status/SensorConfig/Acceleration/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/Acceleration/ */

		} /* case */
			//pop Acceleration

		case 5: //stateComplexBegin(Audio)
		{
			label = 6; //assert 6==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_audio(cur);
					push(next);
				}

				//push Audio: next 8

			} else {
				label = 8;
				continue;
			} /* Audio */

			break;
		} /* stateEnd */

		case 6: //stateBegin(rate)
		{
			label = 7; //assert 7==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 7: //stateComplexEnd(Audio) : 0..1
		{

			label = 5; /* constLoopEnd1 /Status/SensorConfig/Audio/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/Audio/ */

		} /* case */
			//pop Audio

		case 8: //stateComplexBegin(Light)
		{
			label = 9; //assert 9==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_light(cur);
					push(next);
				}

				//push Light: next 11

			} else {
				label = 11;
				continue;
			} /* Light */

			break;
		} /* stateEnd */

		case 9: //stateBegin(rate)
		{
			label = 10; //assert 10==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 10: //stateComplexEnd(Light) : 0..1
		{

			label = 8; /* constLoopEnd1 /Status/SensorConfig/Light/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/Light/ */

		} /* case */
			//pop Light

		case 11: //stateComplexBegin(AmbientLight)
		{
			label = 12; //assert 12==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_ambientLight(cur);
					push(next);
				}

				//push AmbientLight: next 14

			} else {
				label = 14;
				continue;
			} /* AmbientLight */

			break;
		} /* stateEnd */

		case 12: //stateBegin(rate)
		{
			label = 13; //assert 13==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 13: //stateComplexEnd(AmbientLight) : 0..1
		{

			label = 11; /* constLoopEnd1 /Status/SensorConfig/AmbientLight/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/AmbientLight/ */

		} /* case */
			//pop AmbientLight

		case 14: //stateComplexBegin(Force)
		{
			label = 15; //assert 15==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_force(cur);
					push(next);
				}

				//push Force: next 17

			} else {
				label = 17;
				continue;
			} /* Force */

			break;
		} /* stateEnd */

		case 15: //stateBegin(rate)
		{
			label = 16; //assert 16==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 16: //stateComplexEnd(Force) : 0..1
		{

			label = 14; /* constLoopEnd1 /Status/SensorConfig/Force/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/Force/ */

		} /* case */
			//pop Force

		case 17: //stateComplexBegin(Temperature)
		{
			label = 18; //assert 18==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_temperature(cur);
					push(next);
				}

				//push Temperature: next 20

			} else {
				label = 20;
				continue;
			} /* Temperature */

			break;
		} /* stateEnd */

		case 18: //stateBegin(rate)
		{
			label = 19; //assert 19==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 19: //stateComplexEnd(Temperature) : 0..1
		{

			label = 17; /* constLoopEnd1 /Status/SensorConfig/Temperature/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/SensorConfig/Temperature/ */

		} /* case */
			//pop Temperature

		case 20: //stateComplexEnd(SensorConfig) : 1..1
		{

			label = 21; // Complex End

			if (pop())
				;

			break;

		} /* case */
			//pop SensorConfig

		case 21: //stateComplexBegin(AllSensorConfig)
		{
			label = 22; //assert 22==label+1

			/* lowerbound=1 upperbound=1*/

			{
				sens_SSimpStatus * cur;
				cur = (sens_SSimpStatus *) top();
				push(&(cur->allSensorConfig));
			}

			//push AllSensorConfig: next 

			break;
		} /* stateEnd */

		case 22: //stateComplexBegin(Acceleration)
		{
			label = 23; //assert 23==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_acceleration(cur);
					push(next);
				}

				//push Acceleration: next 25

			} else {
				label = 25;
				continue;
			} /* Acceleration */

			break;
		} /* stateEnd */

		case 23: //stateBegin(rate)
		{
			label = 24; //assert 24==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 24: //stateComplexEnd(Acceleration) : 0..1
		{

			label = 22; /* constLoopEnd1 /Status/AllSensorConfig/Acceleration/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/Acceleration/ */

		} /* case */
			//pop Acceleration

		case 25: //stateComplexBegin(Audio)
		{
			label = 26; //assert 26==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_audio(cur);
					push(next);
				}

				//push Audio: next 28

			} else {
				label = 28;
				continue;
			} /* Audio */

			break;
		} /* stateEnd */

		case 26: //stateBegin(rate)
		{
			label = 27; //assert 27==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 27: //stateComplexEnd(Audio) : 0..1
		{

			label = 25; /* constLoopEnd1 /Status/AllSensorConfig/Audio/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/Audio/ */

		} /* case */
			//pop Audio

		case 28: //stateComplexBegin(Light)
		{
			label = 29; //assert 29==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_light(cur);
					push(next);
				}

				//push Light: next 31

			} else {
				label = 31;
				continue;
			} /* Light */

			break;
		} /* stateEnd */

		case 29: //stateBegin(rate)
		{
			label = 30; //assert 30==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 30: //stateComplexEnd(Light) : 0..1
		{

			label = 28; /* constLoopEnd1 /Status/AllSensorConfig/Light/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/Light/ */

		} /* case */
			//pop Light

		case 31: //stateComplexBegin(AmbientLight)
		{
			label = 32; //assert 32==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_ambientLight(cur);
					push(next);
				}

				//push AmbientLight: next 34

			} else {
				label = 34;
				continue;
			} /* AmbientLight */

			break;
		} /* stateEnd */

		case 32: //stateBegin(rate)
		{
			label = 33; //assert 33==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 33: //stateComplexEnd(AmbientLight) : 0..1
		{

			label = 31; /* constLoopEnd1 /Status/AllSensorConfig/AmbientLight/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/AmbientLight/ */

		} /* case */
			//pop AmbientLight

		case 34: //stateComplexBegin(Force)
		{
			label = 35; //assert 35==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_force(cur);
					push(next);
				}

				//push Force: next 37

			} else {
				label = 37;
				continue;
			} /* Force */

			break;
		} /* stateEnd */

		case 35: //stateBegin(rate)
		{
			label = 36; //assert 36==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 36: //stateComplexEnd(Force) : 0..1
		{

			label = 34; /* constLoopEnd1 /Status/AllSensorConfig/Force/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/Force/ */

		} /* case */
			//pop Force

		case 37: //stateComplexBegin(Temperature)
		{
			label = 38; //assert 38==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 2*/

				{
					sens_SensorConfigurationType * cur;
					sens_SSimpRateConfig * next;

					cur = (sens_SensorConfigurationType *) top();
					next = sens_SensorConfigurationType_add_temperature(cur);
					push(next);
				}

				//push Temperature: next 40

			} else {
				label = 40;
				continue;
			} /* Temperature */

			break;
		} /* stateEnd */

		case 38: //stateBegin(rate)
		{
			label = 39; //assert 39==label+1

			{

				{
					sens_SSimpRateConfig * cur;
					cur = (sens_SSimpRateConfig *) top();
					push(&(cur->rate));
				}

				{
					sens_Byte* cur;
					cur = (sens_Byte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-128));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 39: //stateComplexEnd(Temperature) : 0..1
		{

			label = 37; /* constLoopEnd1 /Status/AllSensorConfig/Temperature/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Status/AllSensorConfig/Temperature/ */

		} /* case */
			//pop Temperature

		case 40: //stateComplexEnd(AllSensorConfig) : 1..1
		{

			label = 41; // Complex End

			if (pop())
				;

			break;

		} /* case */
			//pop AllSensorConfig

		case 41: //stateBegin(BatteryVoltage)
		{
			label = 42; //assert 42==label+1

			{

				{
					sens_SSimpStatus * cur;
					cur = (sens_SSimpStatus *) top();
					push(&(cur->batteryVoltage));
				}

				{
					sens_UnsignedShort* cur;
					cur = (sens_UnsignedShort*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(uint16_t*) cur = (((c) * 1) + (0))
						;
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 42: //stateBegin(CurrentTime)
		{
			label = 43; //assert 43==label+1

			{

				{
					sens_SSimpStatus * cur;
					cur = (sens_SSimpStatus *) top();
					push(&(cur->currentTime));
				}

				{
					sens_DateTime* cur;
					cur = (sens_DateTime*) top();

					{
						read_bits(reader, (u_char *) &(cur)->tv_sec, 32);

						{
							uint16_t usec = 0;
							read_bits(reader, (u_char *) &usec, 12);
							(cur)->tv_usec = (uint32_t) usec << 4;
						}

					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 43: //stateBegin(UpTime)
		{
			label = 44; //assert 44==label+1

			{

				{
					sens_SSimpStatus * cur;
					cur = (sens_SSimpStatus *) top();
					push(&(cur->upTime));
				}

				{
					sens_DateTime* cur;
					cur = (sens_DateTime*) top();

					{
						read_bits(reader, (u_char *) &(cur)->tv_sec, 32);

						{
							uint16_t usec = 0;
							read_bits(reader, (u_char *) &usec, 12);
							(cur)->tv_usec = (uint32_t) usec << 4;
						}

					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 44: //stateComplexEnd(Status) : 1..1
		{

			label = 45; // Complex End

			if (pop())
				;

			break;

		} /* case */
			//pop Status

		default: //StopState

			return 0;

		}
	}

	return 1;
}

