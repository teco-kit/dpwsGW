
/* Generated file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>

#include <bitsio/read_bits.h>

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

static int gsoap_automata(struct READER_STRUCT *reader, struct soap *soap,
		int *label);

int Sample_bin2sax_run(struct READER_STRUCT *reader, struct soap *soap) {
	int ret;
	int label = 0;
	while ((ret = gsoap_automata(reader, soap, &label)) > 0)
		;

	return ret;
}

// return:  0 if EOF, 0<for fault, and  read bits else (TODO)
static int gsoap_automata(struct READER_STRUCT *reader, struct soap *soap,
		int *_label) {
#define label (*_label)

	int ret = 0;
	char *str = NULL;

	while (1) {
		switch (label) {

		// Start State


		case 0://stateComplexBegin(Sample)
		{
			label = 1; //assert 1==label+1

			/* lowerbound=1 upperbound=1*/

			soap_element_begin_out(soap, "sens:Sample", 0, "");

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
					struct timeval t;
					struct tm res;
					char out[64];

					{
						read_bits(reader, (u_char *) &(&t)->tv_sec, 32);

						{
							uint16_t usec = 0;
							read_bits(reader, (u_char *) &usec, 12);
							(&t)->tv_usec = (uint32_t) usec << 4;
						}

					};

					ret = strftime(out, sizeof(out), "%Y-%m-%dT%H:%M:%S",
							localtime_r(&t.tv_sec, &res));

					if (ret > 0) {
						ret = asprintf(&str, "%s.%06uZ", out, (uint32_t)(
								(((uint64_t) t.tv_usec) * 1000000) >> 16));
					} else {
						ret = -1;
					}
				};

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * TimeStamp_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:TimeStamp", -1, &TimeStamp_str,
							"", SOAP_TYPE_string);
				}

				free(str);

			} /* TimeStamp */

			break;
		} /* stateEnd */

		case 2://stateComplexBegin(Accelleration)
		{
			label = 3; //assert 3==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				soap_element_begin_out(soap, "sens:Accelleration", 0, "");

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
					uint16_t c;
					ret = read_bits(reader, (u_char *) &c, 16);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%d", (int16_t)(((c) * 1)
								+ (-32768))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * x_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:x", -1, &x_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			}

			break;
		} /* stateEnd */

		case 4: //stateBegin(y)
		{
			label = 5; //assert 5==label+1


			{

				{
					uint16_t c;
					ret = read_bits(reader, (u_char *) &c, 16);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%d", (int16_t)(((c) * 1)
								+ (-32768))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * y_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:y", -1, &y_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			}

			break;
		} /* stateEnd */

		case 5: //stateBegin(z)
		{
			label = 6; //assert 6==label+1


			{

				{
					uint16_t c;
					ret = read_bits(reader, (u_char *) &c, 16);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%d", (int16_t)(((c) * 1)
								+ (-32768))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * z_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:z", -1, &z_str, "",
							SOAP_TYPE_string);
				}

				free(str);

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret
								= asprintf(&str, "%d", (int8_t)(((c) * 1)
										+ (-128))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * index_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:index", -1, &index_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			} /* index */

			break;
		} /* stateEnd */

		case 7: //stateComplexEnd(Accelleration) : 0..1
		{

			label = 2; /* constLoopEnd1 /Sample/Accelleration/ */

			soap_element_end_out(soap, "sens:Accelleration");

			continue; /* constLoopEnd2 /Sample/Accelleration/ */

		} /* case */
			//pop Accelleration


		case 8://stateComplexBegin(Audio)
		{
			label = 9; //assert 9==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				soap_element_begin_out(soap, "sens:Audio", 0, "");

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%u", (((c) * 1) + (0))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * volume_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:volume", -1, &volume_str, "",
							SOAP_TYPE_string);
				}

				free(str);

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret
								= asprintf(&str, "%d", (int8_t)(((c) * 1)
										+ (-128))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * index_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:index", -1, &index_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			} /* index */

			break;
		} /* stateEnd */

		case 11: //stateComplexEnd(Audio) : 0..1
		{

			label = 8; /* constLoopEnd1 /Sample/Audio/ */

			soap_element_end_out(soap, "sens:Audio");

			continue; /* constLoopEnd2 /Sample/Audio/ */

		} /* case */
			//pop Audio


		case 12://stateComplexBegin(Light)
		{
			label = 13; //assert 13==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				soap_element_begin_out(soap, "sens:Light", 0, "");

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
					uint16_t c;
					ret = read_bits(reader, (u_char *) &c, 16);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%u", (((c) * 1) + (0))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * daylight_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:daylight", -1, &daylight_str,
							"", SOAP_TYPE_string);
				}

				free(str);

			}

			break;
		} /* stateEnd */

		case 14: //stateBegin(infrared)
		{
			label = 15; //assert 15==label+1


			{

				{
					uint16_t c;
					ret = read_bits(reader, (u_char *) &c, 16);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%u", (((c) * 1) + (0))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * infrared_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:infrared", -1, &infrared_str,
							"", SOAP_TYPE_string);
				}

				free(str);

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret
								= asprintf(&str, "%d", (int8_t)(((c) * 1)
										+ (-128))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * index_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:index", -1, &index_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			} /* index */

			break;
		} /* stateEnd */

		case 16: //stateComplexEnd(Light) : 0..1
		{

			label = 12; /* constLoopEnd1 /Sample/Light/ */

			soap_element_end_out(soap, "sens:Light");

			continue; /* constLoopEnd2 /Sample/Light/ */

		} /* case */
			//pop Light


		case 17://stateComplexBegin(Force)
		{
			label = 18; //assert 18==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				soap_element_begin_out(soap, "sens:Force", 0, "");

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%u", (((c) * 1) + (0))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * value_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:value", -1, &value_str, "",
							SOAP_TYPE_string);
				}

				free(str);

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret
								= asprintf(&str, "%d", (int8_t)(((c) * 1)
										+ (-128))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * index_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:index", -1, &index_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			} /* index */

			break;
		} /* stateEnd */

		case 20: //stateComplexEnd(Force) : 0..1
		{

			label = 17; /* constLoopEnd1 /Sample/Force/ */

			soap_element_end_out(soap, "sens:Force");

			continue; /* constLoopEnd2 /Sample/Force/ */

		} /* case */
			//pop Force


		case 21://stateComplexBegin(Temperature)
		{
			label = 22; //assert 22==label+1

			/* lowerbound=0 upperbound=1*/

			if (read_bit(reader)) { /* depth 1*/

				soap_element_begin_out(soap, "sens:Temperature", 0, "");

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 7);
					if (ret >= 0) //todo check for exact
					{
						ret = asprintf(&str, "%d", (int8_t)(((c) * 1) + (-20))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * value_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:value", -1, &value_str, "",
							SOAP_TYPE_string);
				}

				free(str);

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
					uint8_t c;
					ret = read_bits(reader, (u_char *) &c, 8);
					if (ret >= 0) //todo check for exact
					{
						ret
								= asprintf(&str, "%d", (int8_t)(((c) * 1)
										+ (-128))); //TODO: check overflows
					} else {
						ret = -1;
					}
				}

				;

				if (ret < 0) {
					fprintf(stderr, "Error reading input data. Errno: %s\n",
							strerror(errno));
					return -1;
				}

				{
					char * index_str = soap_strdup(soap, str);
					soap_outstring(soap, "sens:index", -1, &index_str, "",
							SOAP_TYPE_string);
				}

				free(str);

			} /* index */

			break;
		} /* stateEnd */

		case 24: //stateComplexEnd(Temperature) : 0..1
		{

			label = 21; /* constLoopEnd1 /Sample/Temperature/ */

			soap_element_end_out(soap, "sens:Temperature");

			continue; /* constLoopEnd2 /Sample/Temperature/ */

		} /* case */
			//pop Temperature


		case 25: //stateComplexEnd(Sample) : 1..1
		{

			label = 26; // Complex End


			soap_element_end_out(soap, "sens:Sample");

			break;

		} /* case */
			//pop Sample


		default: //StopState


			return 0;

		}
	}

	return 1;
}

