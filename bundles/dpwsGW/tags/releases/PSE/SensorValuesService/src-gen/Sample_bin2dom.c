
#include "Sample_bin2dom.h"
#include <ptr_stack.h>
#include <string.h>
#include <bitsio/read_bits_buf.h>

int Sample_bin2dom_run(struct READER_STRUCT *reader, sens_SSimpSample *dom) {
	int label = 0;
	new_ptr_stack(stack, dom);

	memset(dom, 0, sizeof(*dom));

	for (;;) {
		switch (label) {

		// Start State


		case 0://stateComplexBegin(Sample)
		{
			label = 1; //assert 1==label+1

			/* lowerbound=1 upperbound=1*/

			//push(pop()): preinitialized stack


			//push Sample: next 


			break;
		} /* stateEnd */

		case 1: //stateBegin(TimeStamp)
		{
			label = 2; //assert 2==label+1


			/* loop /SampleTimeStamp label=label */
			while (read_bit(reader))

			{

				{
					sens_SSimpSample * cur;
					sens_DateTime * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_timeStamp(cur);
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

			} /* TimeStamp */

			break;
		} /* stateEnd */

		case 2://stateComplexBegin(Accelleration)
		{
			label = 3; //assert 3==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpSample * cur;
					sens_ADXL210Sample * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_accelleration(cur);
					push(next);
				}

				//push Accelleration: next 8


			} else {
				label = 8;
				continue;
			} /* Accelleration */

			break;
		} /* stateEnd */

		case 3: //stateBegin(x)
		{
			label = 4; //assert 4==label+1


			{

				{
					sens_ADXL210Sample * cur;
					cur = (sens_ADXL210Sample *) top();
					push(&(cur->x));
				}

				{
					sens_XType* cur;
					cur = (sens_XType*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(int16_t*) cur = (int16_t)(((c) * 1) + (-32768));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 4: //stateBegin(y)
		{
			label = 5; //assert 5==label+1


			{

				{
					sens_ADXL210Sample * cur;
					cur = (sens_ADXL210Sample *) top();
					push(&(cur->y));
				}

				{
					sens_YType* cur;
					cur = (sens_YType*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(int16_t*) cur = (int16_t)(((c) * 1) + (-32768));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 5: //stateBegin(z)
		{
			label = 6; //assert 6==label+1


			{

				{
					sens_ADXL210Sample * cur;
					cur = (sens_ADXL210Sample *) top();
					push(&(cur->z));
				}

				{
					sens_ZType* cur;
					cur = (sens_ZType*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(int16_t*) cur = (int16_t)(((c) * 1) + (-32768));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 6: //stateBegin(index)
		{
			label = 7; //assert 7==label+1


			/* loop /Sample/Accellerationindex label=label */
			while (read_bit(reader))

			{

				{
					sens_ADXL210Sample * cur;
					sens_Byte * next;

					cur = (sens_ADXL210Sample *) top();
					next = sens_ADXL210Sample_add_index(cur);
					push(next);
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

			} /* index */

			break;
		} /* stateEnd */

		case 7: //stateComplexEnd(Accelleration) : 0..1
		{

			label = 2; /* constLoopEnd1 /Sample/Accelleration/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Sample/Accelleration/ */

		} /* case */
			//pop Accelleration


		case 8://stateComplexBegin(Audio)
		{
			label = 9; //assert 9==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpSample * cur;
					sens_SP101Sample * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_audio(cur);
					push(next);
				}

				//push Audio: next 12


			} else {
				label = 12;
				continue;
			} /* Audio */

			break;
		} /* stateEnd */

		case 9: //stateBegin(volume)
		{
			label = 10; //assert 10==label+1


			{

				{
					sens_SP101Sample * cur;
					cur = (sens_SP101Sample *) top();
					push(&(cur->volume));
				}

				{
					sens_UnsignedByte* cur;
					cur = (sens_UnsignedByte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(uint8_t*) cur = (((c) * 1) + (0));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 10: //stateBegin(index)
		{
			label = 11; //assert 11==label+1


			/* loop /Sample/Audioindex label=label */
			while (read_bit(reader))

			{

				{
					sens_SP101Sample * cur;
					sens_Byte * next;

					cur = (sens_SP101Sample *) top();
					next = sens_SP101Sample_add_index(cur);
					push(next);
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

			} /* index */

			break;
		} /* stateEnd */

		case 11: //stateComplexEnd(Audio) : 0..1
		{

			label = 8; /* constLoopEnd1 /Sample/Audio/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Sample/Audio/ */

		} /* case */
			//pop Audio


		case 12://stateComplexBegin(Light)
		{
			label = 13; //assert 13==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpSample * cur;
					sens_TSL2550Sample * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_light(cur);
					push(next);
				}

				//push Light: next 17


			} else {
				label = 17;
				continue;
			} /* Light */

			break;
		} /* stateEnd */

		case 13: //stateBegin(daylight)
		{
			label = 14; //assert 14==label+1


			{

				{
					sens_TSL2550Sample * cur;
					cur = (sens_TSL2550Sample *) top();
					push(&(cur->daylight));
				}

				{
					sens_UnsignedShort* cur;
					cur = (sens_UnsignedShort*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(uint16_t*) cur = (((c) * 1) + (0));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 14: //stateBegin(infrared)
		{
			label = 15; //assert 15==label+1


			{

				{
					sens_TSL2550Sample * cur;
					cur = (sens_TSL2550Sample *) top();
					push(&(cur->infrared));
				}

				{
					sens_UnsignedShort* cur;
					cur = (sens_UnsignedShort*) top();

					{
						uint16_t c;
						read_bits(reader, (u_char *) &c, 16);
						*(uint16_t*) cur = (((c) * 1) + (0));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 15: //stateBegin(index)
		{
			label = 16; //assert 16==label+1


			/* loop /Sample/Lightindex label=label */
			while (read_bit(reader))

			{

				{
					sens_TSL2550Sample * cur;
					sens_Byte * next;

					cur = (sens_TSL2550Sample *) top();
					next = sens_TSL2550Sample_add_index(cur);
					push(next);
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

			} /* index */

			break;
		} /* stateEnd */

		case 16: //stateComplexEnd(Light) : 0..1
		{

			label = 12; /* constLoopEnd1 /Sample/Light/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Sample/Light/ */

		} /* case */
			//pop Light


		case 17://stateComplexBegin(Force)
		{
			label = 18; //assert 18==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpSample * cur;
					sens_FSR152Sample * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_force(cur);
					push(next);
				}

				//push Force: next 21


			} else {
				label = 21;
				continue;
			} /* Force */

			break;
		} /* stateEnd */

		case 18: //stateBegin(value)
		{
			label = 19; //assert 19==label+1


			{

				{
					sens_FSR152Sample * cur;
					cur = (sens_FSR152Sample *) top();
					push(&(cur->value));
				}

				{
					sens_UnsignedByte* cur;
					cur = (sens_UnsignedByte*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 8);
						*(uint8_t*) cur = (((c) * 1) + (0));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 19: //stateBegin(index)
		{
			label = 20; //assert 20==label+1


			/* loop /Sample/Forceindex label=label */
			while (read_bit(reader))

			{

				{
					sens_FSR152Sample * cur;
					sens_Byte * next;

					cur = (sens_FSR152Sample *) top();
					next = sens_FSR152Sample_add_index(cur);
					push(next);
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

			} /* index */

			break;
		} /* stateEnd */

		case 20: //stateComplexEnd(Force) : 0..1
		{

			label = 17; /* constLoopEnd1 /Sample/Force/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Sample/Force/ */

		} /* case */
			//pop Force


		case 21://stateComplexBegin(Temperature)
		{
			label = 22; //assert 22==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				{
					sens_SSimpSample * cur;
					sens_TC74Sample * next;

					cur = (sens_SSimpSample *) top();
					next = sens_SSimpSample_add_temperature(cur);
					push(next);
				}

				//push Temperature: next 25


			} else {
				label = 25;
				continue;
			} /* Temperature */

			break;
		} /* stateEnd */

		case 22: //stateBegin(value)
		{
			label = 23; //assert 23==label+1


			{

				{
					sens_TC74Sample * cur;
					cur = (sens_TC74Sample *) top();
					push(&(cur->value));
				}

				{
					sens_ValueType* cur;
					cur = (sens_ValueType*) top();

					{
						uint8_t c;
						read_bits(reader, (u_char *) &c, 7);
						*(int8_t*) cur = (int8_t)(((c) * 1) + (-20));
					}

				}

				if (pop())
					;

			}

			break;
		} /* stateEnd */

		case 23: //stateBegin(index)
		{
			label = 24; //assert 24==label+1


			/* loop /Sample/Temperatureindex label=label */
			while (read_bit(reader))

			{

				{
					sens_TC74Sample * cur;
					sens_Byte * next;

					cur = (sens_TC74Sample *) top();
					next = sens_TC74Sample_add_index(cur);
					push(next);
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

			} /* index */

			break;
		} /* stateEnd */

		case 24: //stateComplexEnd(Temperature) : 0..1
		{

			label = 21; /* constLoopEnd1 /Sample/Temperature/ */

			if (pop())
				;

			continue; /* constLoopEnd2 /Sample/Temperature/ */

		} /* case */
			//pop Temperature


		case 25: //stateComplexEnd(Sample) : 1..1
		{

			label = 26; // Complex End


			if (pop())
				;

			break;

		} /* case */
			//pop Sample


		default: //StopState


			return 0;

		}
	}

	return 1;
}

