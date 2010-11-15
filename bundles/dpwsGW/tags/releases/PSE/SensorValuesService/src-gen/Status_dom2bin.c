
#include "Status_dom2bin.h"

int Status_dom2bin_run(struct sens_SSimpStatus *dom,
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


			case 0://stateComplexBegin(Status)
			{
				label = 1; //assert 1==label+1

				/* lowerbound=1 upperbound=1*/

				//assert preinitialized stack 


				//push Status: next 


				break;
			} /* stateEnd */

			case 1://stateComplexBegin(SensorConfig)
			{
				label = 2; //assert 2==label+1

				/* lowerbound=1 upperbound=1*/

				{
					sens_SSimpStatus *parent;
					sens_SensorConfigurationType *cur;
					parent = (sens_SSimpStatus*) top();
					cur = &(parent->sensorConfig);
					push(cur);
				}

				//push SensorConfig: next 


				break;
			} /* stateEnd */

			case 2://stateComplexBegin(Acceleration)
			{
				label = 3; //assert 3==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_acceleration(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Acceleration: next 5


						push(cur);
						write_true(writer);
					} else {
						label = 5; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Acceleration */
				}

				break;
			} /* stateEnd */

			case 3: //stateBegin(rate)
			{
				label = 4; //assert 4==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 4: //stateComplexEnd(Acceleration) : 0..1
			{

				label = 2; /* constLoopEnd1 /Status/SensorConfig/Acceleration/ */

				//stateNameEnd Acceleration


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/Acceleration/ */

			} /* case */
				//pop Acceleration


			case 5://stateComplexBegin(Audio)
			{
				label = 6; //assert 6==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_audio(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Audio: next 8


						push(cur);
						write_true(writer);
					} else {
						label = 8; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Audio */
				}

				break;
			} /* stateEnd */

			case 6: //stateBegin(rate)
			{
				label = 7; //assert 7==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 7: //stateComplexEnd(Audio) : 0..1
			{

				label = 5; /* constLoopEnd1 /Status/SensorConfig/Audio/ */

				//stateNameEnd Audio


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/Audio/ */

			} /* case */
				//pop Audio


			case 8://stateComplexBegin(Light)
			{
				label = 9; //assert 9==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_light(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Light: next 11


						push(cur);
						write_true(writer);
					} else {
						label = 11; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Light */
				}

				break;
			} /* stateEnd */

			case 9: //stateBegin(rate)
			{
				label = 10; //assert 10==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 10: //stateComplexEnd(Light) : 0..1
			{

				label = 8; /* constLoopEnd1 /Status/SensorConfig/Light/ */

				//stateNameEnd Light


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/Light/ */

			} /* case */
				//pop Light


			case 11://stateComplexBegin(AmbientLight)
			{
				label = 12; //assert 12==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_ambientLight(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push AmbientLight: next 14


						push(cur);
						write_true(writer);
					} else {
						label = 14; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* AmbientLight */
				}

				break;
			} /* stateEnd */

			case 12: //stateBegin(rate)
			{
				label = 13; //assert 13==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 13: //stateComplexEnd(AmbientLight) : 0..1
			{

				label = 11; /* constLoopEnd1 /Status/SensorConfig/AmbientLight/ */

				//stateNameEnd AmbientLight


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/AmbientLight/ */

			} /* case */
				//pop AmbientLight


			case 14://stateComplexBegin(Force)
			{
				label = 15; //assert 15==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_force(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Force: next 17


						push(cur);
						write_true(writer);
					} else {
						label = 17; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Force */
				}

				break;
			} /* stateEnd */

			case 15: //stateBegin(rate)
			{
				label = 16; //assert 16==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 16: //stateComplexEnd(Force) : 0..1
			{

				label = 14; /* constLoopEnd1 /Status/SensorConfig/Force/ */

				//stateNameEnd Force


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/Force/ */

			} /* case */
				//pop Force


			case 17://stateComplexBegin(Temperature)
			{
				label = 18; //assert 18==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_temperature(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Temperature: next 20


						push(cur);
						write_true(writer);
					} else {
						label = 20; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Temperature */
				}

				break;
			} /* stateEnd */

			case 18: //stateBegin(rate)
			{
				label = 19; //assert 19==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 19: //stateComplexEnd(Temperature) : 0..1
			{

				label = 17; /* constLoopEnd1 /Status/SensorConfig/Temperature/ */

				//stateNameEnd Temperature


				last = pop();

				continue; /* constLoopEnd2 /Status/SensorConfig/Temperature/ */

			} /* case */
				//pop Temperature


			case 20: //stateComplexEnd(SensorConfig) : 1..1
			{

				if (pop())
					;

				label = 21; // Complex End


				//stateNameEnd SensorConfig


				break;

			} /* case */
				//pop SensorConfig


			case 21://stateComplexBegin(AllSensorConfig)
			{
				label = 22; //assert 22==label+1

				/* lowerbound=1 upperbound=1*/

				{
					sens_SSimpStatus *parent;
					sens_SensorConfigurationType *cur;
					parent = (sens_SSimpStatus*) top();
					cur = &(parent->allSensorConfig);
					push(cur);
				}

				//push AllSensorConfig: next 


				break;
			} /* stateEnd */

			case 22://stateComplexBegin(Acceleration)
			{
				label = 23; //assert 23==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_acceleration(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Acceleration: next 25


						push(cur);
						write_true(writer);
					} else {
						label = 25; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Acceleration */
				}

				break;
			} /* stateEnd */

			case 23: //stateBegin(rate)
			{
				label = 24; //assert 24==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 24: //stateComplexEnd(Acceleration) : 0..1
			{

				label = 22; /* constLoopEnd1 /Status/AllSensorConfig/Acceleration/ */

				//stateNameEnd Acceleration


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/Acceleration/ */

			} /* case */
				//pop Acceleration


			case 25://stateComplexBegin(Audio)
			{
				label = 26; //assert 26==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_audio(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Audio: next 28


						push(cur);
						write_true(writer);
					} else {
						label = 28; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Audio */
				}

				break;
			} /* stateEnd */

			case 26: //stateBegin(rate)
			{
				label = 27; //assert 27==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 27: //stateComplexEnd(Audio) : 0..1
			{

				label = 25; /* constLoopEnd1 /Status/AllSensorConfig/Audio/ */

				//stateNameEnd Audio


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/Audio/ */

			} /* case */
				//pop Audio


			case 28://stateComplexBegin(Light)
			{
				label = 29; //assert 29==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_light(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Light: next 31


						push(cur);
						write_true(writer);
					} else {
						label = 31; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Light */
				}

				break;
			} /* stateEnd */

			case 29: //stateBegin(rate)
			{
				label = 30; //assert 30==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 30: //stateComplexEnd(Light) : 0..1
			{

				label = 28; /* constLoopEnd1 /Status/AllSensorConfig/Light/ */

				//stateNameEnd Light


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/Light/ */

			} /* case */
				//pop Light


			case 31://stateComplexBegin(AmbientLight)
			{
				label = 32; //assert 32==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_ambientLight(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push AmbientLight: next 34


						push(cur);
						write_true(writer);
					} else {
						label = 34; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* AmbientLight */
				}

				break;
			} /* stateEnd */

			case 32: //stateBegin(rate)
			{
				label = 33; //assert 33==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 33: //stateComplexEnd(AmbientLight) : 0..1
			{

				label = 31; /* constLoopEnd1 /Status/AllSensorConfig/AmbientLight/ */

				//stateNameEnd AmbientLight


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/AmbientLight/ */

			} /* case */
				//pop AmbientLight


			case 34://stateComplexBegin(Force)
			{
				label = 35; //assert 35==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur = sens_SensorConfigurationType_next_force(
							(sens_SensorConfigurationType *) top(),
							(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Force: next 37


						push(cur);
						write_true(writer);
					} else {
						label = 37; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Force */
				}

				break;
			} /* stateEnd */

			case 35: //stateBegin(rate)
			{
				label = 36; //assert 36==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 36: //stateComplexEnd(Force) : 0..1
			{

				label = 34; /* constLoopEnd1 /Status/AllSensorConfig/Force/ */

				//stateNameEnd Force


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/Force/ */

			} /* case */
				//pop Force


			case 37://stateComplexBegin(Temperature)
			{
				label = 38; //assert 38==label+1

				/* lowerbound=0 upperbound=1*/

				{
					sens_SSimpRateConfig *cur;
					if (NULL != (cur
							= sens_SensorConfigurationType_next_temperature(
									(sens_SensorConfigurationType *) top(),
									(sens_SSimpRateConfig *) last))) { /* depth 2*/

						//push Temperature: next 40


						push(cur);
						write_true(writer);
					} else {
						label = 40; //TODO: xx
						write_false(writer);
						last = NULL;
						continue;
					} /* Temperature */
				}

				break;
			} /* stateEnd */

			case 38: //stateBegin(rate)
			{
				label = 39; //assert 39==label+1


				{
					sens_SSimpRateConfig *parent;
					sens_Byte *cur;
					parent = (sens_SSimpRateConfig*) top();
					cur = &(parent->rate);
					push(cur);
				}

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

					//stateNameEnd rate


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 39: //stateComplexEnd(Temperature) : 0..1
			{

				label = 37; /* constLoopEnd1 /Status/AllSensorConfig/Temperature/ */

				//stateNameEnd Temperature


				last = pop();

				continue; /* constLoopEnd2 /Status/AllSensorConfig/Temperature/ */

			} /* case */
				//pop Temperature


			case 40: //stateComplexEnd(AllSensorConfig) : 1..1
			{

				if (pop())
					;

				label = 41; // Complex End


				//stateNameEnd AllSensorConfig


				break;

			} /* case */
				//pop AllSensorConfig


			case 41: //stateBegin(BatteryVoltage)
			{
				label = 42; //assert 42==label+1


				{
					sens_SSimpStatus *parent;
					sens_UnsignedShort *cur;
					parent = (sens_SSimpStatus*) top();
					cur = &(parent->batteryVoltage);
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

					//stateNameEnd BatteryVoltage


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 42: //stateBegin(CurrentTime)
			{
				label = 43; //assert 43==label+1


				{
					sens_SSimpStatus *parent;
					sens_DateTime *cur;
					parent = (sens_SSimpStatus*) top();
					cur = &(parent->currentTime);
					push(cur);
				}

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
								ret += write_bits(writer, (u_char *) &usec, 12);
							}

						}

					}

					//stateNameEnd CurrentTime


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 43: //stateBegin(UpTime)
			{
				label = 44; //assert 44==label+1


				{
					sens_SSimpStatus *parent;
					sens_DateTime *cur;
					parent = (sens_SSimpStatus*) top();
					cur = &(parent->upTime);
					push(cur);
				}

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
								ret += write_bits(writer, (u_char *) &usec, 12);
							}

						}

					}

					//stateNameEnd UpTime


					if (pop())
						;

				}

				break;
			} /* stateEnd */

			case 44: //stateComplexEnd(Status) : 1..1
			{

				if (pop())
					;

				label = 45; // Complex End


				//stateNameEnd Status


				break;

			} /* case */
				//pop Status


			default: //StopState


				return 0;

			}
		}

		return 1;
	}
}

