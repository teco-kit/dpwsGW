
#include "Sample_dom2bin.h"

int Sample_dom2bin_run(struct sens_SSimpSample *dom,
		struct WRITER_STRUCT *writer) {
	int label = 0;
	new_ptr_stack(stack, dom);

	{
		void *last = NULL;

		for (;;) {
#ifdef debug_dom
			debug_dom(label,top(),stack);
#endif
			switch (label) {

			// Start State


			case 0://stateComplexBegin(Sample)
			{
				label = 1; //assert 1==label+1

				/* lowerbound=1 upperbound=1*/

				//assert preinitialized stack 


				//push Sample: next 


				break;
			} /* stateEnd */

			case 1: //stateBegin(TimeStamp)
			{
				label = 2; //assert 2==label+1


				{
					/* loop /Sample/TimeStamp */
					sens_DateTime *cur;
					while (NULL
							!= (cur = sens_SSimpSample_next_timeStamp(
									(sens_SSimpSample *) top(),
									(sens_DateTime *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_DateTime* val;
								val = (sens_DateTime*) top();

								{
									ret = write_bits(writer,
											(u_char *) &((*val)).tv_sec, 32);

									{
										uint16_t usec;
										usec = ((*val)).tv_usec >> 4;
										ret += write_bits(writer,
												(u_char *) &usec, 12);
									}

								}

							}

							//stateNameEnd TimeStamp


							last = cur;

						} /* TimeStamp */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 2://stateComplexBegin(Accelleration)
			{
				label = 3; //assert 3==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_ADXL210Sample *cur;
					if (NULL != (cur = sens_SSimpSample_next_accelleration(
							(sens_SSimpSample *) top(),
							(sens_ADXL210Sample *) last))) { /* depth 1*/

						//push Accelleration: next 8


						push(cur);
						write_true(writer);
					} else {
						label = 8; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Accelleration */
				}

				break;
			} /* stateEnd */

			case 3: //stateBegin(x)
			{
				label = 4; //assert 4==label+1


				{
					sens_ADXL210Sample *parent;
					sens_XType *cur;
					parent = (sens_ADXL210Sample*) top();
					cur = &(parent->x);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_XType* val;
						val = (sens_XType*) top();

						{
							uint16_t d;
							d = (((*val)) - (-32768)) / 1;
							ret = write_bits(writer, (u_char *) &d, 16);
						}

					}

					//stateNameEnd x


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 4: //stateBegin(y)
			{
				label = 5; //assert 5==label+1


				{
					sens_ADXL210Sample *parent;
					sens_YType *cur;
					parent = (sens_ADXL210Sample*) top();
					cur = &(parent->y);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_YType* val;
						val = (sens_YType*) top();

						{
							uint16_t d;
							d = (((*val)) - (-32768)) / 1;
							ret = write_bits(writer, (u_char *) &d, 16);
						}

					}

					//stateNameEnd y


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 5: //stateBegin(z)
			{
				label = 6; //assert 6==label+1


				{
					sens_ADXL210Sample *parent;
					sens_ZType *cur;
					parent = (sens_ADXL210Sample*) top();
					cur = &(parent->z);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_ZType* val;
						val = (sens_ZType*) top();

						{
							uint16_t d;
							d = (((*val)) - (-32768)) / 1;
							ret = write_bits(writer, (u_char *) &d, 16);
						}

					}

					//stateNameEnd z


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 6: //stateBegin(index)
			{
				label = 7; //assert 7==label+1


				{
					/* loop /Sample/Accelleration/index */
					sens_Byte *cur;
					while (NULL != (cur = sens_ADXL210Sample_next_index(
							(sens_ADXL210Sample *) top(), (sens_Byte *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_Byte* val;
								val = (sens_Byte*) top();

								{
									uint8_t d;
									d = (((*val)) - (-128)) / 1;
									ret = write_bits(writer, (u_char *) &d, 8);
								}

							}

							//stateNameEnd index


							last = cur;

						} /* index */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 7: //stateComplexEnd(Accelleration) : 0..1
			{

				label = 2; /* constLoopEnd1 /Sample/Accelleration/ */

				//stateNameEnd Accelleration


				last = pop();

				continue; /* constLoopEnd2 /Sample/Accelleration/ */

			} /* case */
				//pop Accelleration


			case 8://stateComplexBegin(Audio)
			{
				label = 9; //assert 9==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SP101Sample *cur;
					if (NULL != (cur = sens_SSimpSample_next_audio(
							(sens_SSimpSample *) top(),
							(sens_SP101Sample *) last))) { /* depth 1*/

						//push Audio: next 12


						push(cur);
						write_true(writer);
					} else {
						label = 12; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Audio */
				}

				break;
			} /* stateEnd */

			case 9: //stateBegin(volume)
			{
				label = 10; //assert 10==label+1


				{
					sens_SP101Sample *parent;
					sens_UnsignedByte *cur;
					parent = (sens_SP101Sample*) top();
					cur = &(parent->volume);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_UnsignedByte* val;
						val = (sens_UnsignedByte*) top();

						{
							uint8_t d;
							d = (((*val)) - (0)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

					//stateNameEnd volume


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 10: //stateBegin(index)
			{
				label = 11; //assert 11==label+1


				{
					/* loop /Sample/Audio/index */
					sens_Byte *cur;
					while (NULL != (cur = sens_SP101Sample_next_index(
							(sens_SP101Sample *) top(), (sens_Byte *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_Byte* val;
								val = (sens_Byte*) top();

								{
									uint8_t d;
									d = (((*val)) - (-128)) / 1;
									ret = write_bits(writer, (u_char *) &d, 8);
								}

							}

							//stateNameEnd index


							last = cur;

						} /* index */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 11: //stateComplexEnd(Audio) : 0..1
			{

				label = 8; /* constLoopEnd1 /Sample/Audio/ */

				//stateNameEnd Audio


				last = pop();

				continue; /* constLoopEnd2 /Sample/Audio/ */

			} /* case */
				//pop Audio


			case 12://stateComplexBegin(Light)
			{
				label = 13; //assert 13==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_TSL2550Sample *cur;
					if (NULL != (cur = sens_SSimpSample_next_light(
							(sens_SSimpSample *) top(),
							(sens_TSL2550Sample *) last))) { /* depth 1*/

						//push Light: next 17


						push(cur);
						write_true(writer);
					} else {
						label = 17; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Light */
				}

				break;
			} /* stateEnd */

			case 13: //stateBegin(daylight)
			{
				label = 14; //assert 14==label+1


				{
					sens_TSL2550Sample *parent;
					sens_UnsignedShort *cur;
					parent = (sens_TSL2550Sample*) top();
					cur = &(parent->daylight);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_UnsignedShort* val;
						val = (sens_UnsignedShort*) top();

						{
							uint16_t d;
							d = (((*val)) - (0)) / 1;
							ret = write_bits(writer, (u_char *) &d, 16);
						}

					}

					//stateNameEnd daylight


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 14: //stateBegin(infrared)
			{
				label = 15; //assert 15==label+1


				{
					sens_TSL2550Sample *parent;
					sens_UnsignedShort *cur;
					parent = (sens_TSL2550Sample*) top();
					cur = &(parent->infrared);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_UnsignedShort* val;
						val = (sens_UnsignedShort*) top();

						{
							uint16_t d;
							d = (((*val)) - (0)) / 1;
							ret = write_bits(writer, (u_char *) &d, 16);
						}

					}

					//stateNameEnd infrared


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 15: //stateBegin(index)
			{
				label = 16; //assert 16==label+1


				{
					/* loop /Sample/Light/index */
					sens_Byte *cur;
					while (NULL != (cur = sens_TSL2550Sample_next_index(
							(sens_TSL2550Sample *) top(), (sens_Byte *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_Byte* val;
								val = (sens_Byte*) top();

								{
									uint8_t d;
									d = (((*val)) - (-128)) / 1;
									ret = write_bits(writer, (u_char *) &d, 8);
								}

							}

							//stateNameEnd index


							last = cur;

						} /* index */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 16: //stateComplexEnd(Light) : 0..1
			{

				label = 12; /* constLoopEnd1 /Sample/Light/ */

				//stateNameEnd Light


				last = pop();

				continue; /* constLoopEnd2 /Sample/Light/ */

			} /* case */
				//pop Light


			case 17://stateComplexBegin(Force)
			{
				label = 18; //assert 18==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_FSR152Sample *cur;
					if (NULL != (cur = sens_SSimpSample_next_force(
							(sens_SSimpSample *) top(),
							(sens_FSR152Sample *) last))) { /* depth 1*/

						//push Force: next 21


						push(cur);
						write_true(writer);
					} else {
						label = 21; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Force */
				}

				break;
			} /* stateEnd */

			case 18: //stateBegin(value)
			{
				label = 19; //assert 19==label+1


				{
					sens_FSR152Sample *parent;
					sens_UnsignedByte *cur;
					parent = (sens_FSR152Sample*) top();
					cur = &(parent->value);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_UnsignedByte* val;
						val = (sens_UnsignedByte*) top();

						{
							uint8_t d;
							d = (((*val)) - (0)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

					//stateNameEnd value


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 19: //stateBegin(index)
			{
				label = 20; //assert 20==label+1


				{
					/* loop /Sample/Force/index */
					sens_Byte *cur;
					while (NULL != (cur = sens_FSR152Sample_next_index(
							(sens_FSR152Sample *) top(), (sens_Byte *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_Byte* val;
								val = (sens_Byte*) top();

								{
									uint8_t d;
									d = (((*val)) - (-128)) / 1;
									ret = write_bits(writer, (u_char *) &d, 8);
								}

							}

							//stateNameEnd index


							last = cur;

						} /* index */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 20: //stateComplexEnd(Force) : 0..1
			{

				label = 17; /* constLoopEnd1 /Sample/Force/ */

				//stateNameEnd Force


				last = pop();

				continue; /* constLoopEnd2 /Sample/Force/ */

			} /* case */
				//pop Force


			case 21://stateComplexBegin(Temperature)
			{
				label = 22; //assert 22==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_TC74Sample *cur;
					if (NULL != (cur = sens_SSimpSample_next_temperature(
							(sens_SSimpSample *) top(),
							(sens_TC74Sample *) last))) { /* depth 1*/

						//push Temperature: next 25


						push(cur);
						write_true(writer);
					} else {
						label = 25; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Temperature */
				}

				break;
			} /* stateEnd */

			case 22: //stateBegin(value)
			{
				label = 23; //assert 23==label+1


				{
					sens_TC74Sample *parent;
					sens_ValueType *cur;
					parent = (sens_TC74Sample*) top();
					cur = &(parent->value);
					push(cur);
				}

				{

					{
						ssize_t ret = 0;
						sens_ValueType* val;
						val = (sens_ValueType*) top();

						{
							uint8_t d;
							d = (((*val)) - (-20)) / 1;
							ret = write_bits(writer, (u_char *) &d, 7);
						}

					}

					//stateNameEnd value


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 23: //stateBegin(index)
			{
				label = 24; //assert 24==label+1


				{
					/* loop /Sample/Temperature/index */
					sens_Byte *cur;
					while (NULL != (cur = sens_TC74Sample_next_index(
							(sens_TC74Sample *) top(), (sens_Byte *) last))) {
						push(cur);
						write_true(writer);

						{

							{
								ssize_t ret = 0;
								sens_Byte* val;
								val = (sens_Byte*) top();

								{
									uint8_t d;
									d = (((*val)) - (-128)) / 1;
									ret = write_bits(writer, (u_char *) &d, 8);
								}

							}

							//stateNameEnd index


							last = cur;

						} /* index */

						if (pop())
							;
					}

					last = NULL;
					write_false(writer); //XXX: not necessary, right now symetric to bin2sax...
				}

				break;
			} /* stateEnd */

			case 24: //stateComplexEnd(Temperature) : 0..1
			{

				label = 21; /* constLoopEnd1 /Sample/Temperature/ */

				//stateNameEnd Temperature


				last = pop();

				continue; /* constLoopEnd2 /Sample/Temperature/ */

			} /* case */
				//pop Temperature


			case 25: //stateComplexEnd(Sample) : 1..1
			{

				if (pop())
					;

				label = 26; // Complex End


				//stateNameEnd Sample


				break;

			} /* case */
				//pop Sample


			default: //StopState


				return 0;

			}
		}

		return 1;
	}
}

