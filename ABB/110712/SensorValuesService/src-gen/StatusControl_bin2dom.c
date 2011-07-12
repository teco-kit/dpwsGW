
#include "StatusControl_bin2dom.h"
#include <ptr_stack.h>
#include <string.h>
#include <bitsio/read_bits_buf.h>

int StatusControl_bin2dom_run(struct READER_STRUCT *reader,
		sens_SSimpControl *dom) {
	int label = 0;
	new_ptr_stack(stack, dom);

	memset(dom, 0, sizeof(*dom));

	for (;;) {
		switch (label) {

		// Start State


		case 0://stateComplexBegin(StatusControl)
		{
			label = 1; //assert 1==label+1

			/* lowerbound=1 upperbound=1*/

			//push(pop()): preinitialized stack


			//push StatusControl: next 


			break;
		} /* stateEnd */

		case 1://stateComplexBegin(SensorConfig)
		{
			label = 2; //assert 2==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpControl * cur;
					sens_SensorConfigurationType * next;

					cur = (sens_SSimpControl *) top();
					next = sens_SSimpControl_add_sensorConfig(cur);
					push(next);
				}

				//push SensorConfig: next 21


			} else {
				label = 21;
				continue;
			} /* SensorConfig */

			break;
		} /* stateEnd */

		case 2://stateComplexBegin(Acceleration)
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

			label = 2; /* constLoopEnd1 /StatusControl/SensorConfig/Acceleration/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Acceleration/ */

		} /* case */
			//pop Acceleration


		case 5://stateComplexBegin(Audio)
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

			label = 5; /* constLoopEnd1 /StatusControl/SensorConfig/Audio/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Audio/ */

		} /* case */
			//pop Audio


		case 8://stateComplexBegin(Light)
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

			label = 8; /* constLoopEnd1 /StatusControl/SensorConfig/Light/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Light/ */

		} /* case */
			//pop Light


		case 11://stateComplexBegin(AmbientLight)
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

			label = 11; /* constLoopEnd1 /StatusControl/SensorConfig/AmbientLight/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/AmbientLight/ */

		} /* case */
			//pop AmbientLight


		case 14://stateComplexBegin(Force)
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

			label = 14; /* constLoopEnd1 /StatusControl/SensorConfig/Force/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Force/ */

		} /* case */
			//pop Force


		case 17://stateComplexBegin(Temperature)
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

			label = 17; /* constLoopEnd1 /StatusControl/SensorConfig/Temperature/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Temperature/ */

		} /* case */
			//pop Temperature


		case 20: //stateComplexEnd(SensorConfig) : 0..1
		{

			label = 1; /* constLoopEnd1 /StatusControl/SensorConfig/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/ */

		} /* case */
			//pop SensorConfig


		case 21: //stateBegin(NewTime)
		{
			label = 22; //assert 22==label+1


			/* loop /StatusControlNewTime label=label */
			while (read_bit(reader))

			{

				{
					sens_SSimpControl * cur;
					sens_DateTime * next;

					cur = (sens_SSimpControl *) top();
					next = sens_SSimpControl_add_newTime(cur);
					push(next);
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

			} /* NewTime */

			break;
		} /* stateEnd */

		case 22: //stateComplexEnd(StatusControl) : 1..1
		{

			label = 23; // Complex End


			if (pop())
				;

			break;

		} /* case */
			//pop StatusControl


		default: //StopState


			return 0;

		}
	}

	return 1;
}

