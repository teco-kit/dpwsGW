
/* Generated file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

#include "StatusControl_sax2bin.h"

static int gsoap_automata();
int StatusControl_sax2bin_run(struct soap *soap, struct WRITER_STRUCT *writer) {
	int ret;
	int label = 0;

	while ((ret = gsoap_automata(soap, writer, &label)) > 0)
		;

	return ret;
}

// return:  0 if EOF, 0<for fault, and  read bits else (TODO)
static int gsoap_automata(struct soap *soap, struct WRITER_STRUCT *writer,
		int *_label) {
#define label (*_label)

	for (;;) {
		switch (label) {

// Start State

		case 0: //stateComplexBegin(StatusControl)
		{
			label = 1; //assert 1==label+1

			/* lowerbound=1 upperbound=1*/

			if (!(soap_element_begin_in(soap, "sens:StatusControl", 0, NULL)
					== SOAP_OK)) {
				soap->error =
						soap_sender_fault(
								soap,
								"tag name or namespace mismatch: StatusControl expected",
								NULL);
				//      soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			//push StatusControl: next 

			break;
		} /* stateEnd */

		case 1: //stateComplexBegin(SensorConfig)
		{
			label = 2; //assert 2==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:SensorConfig", 0, NULL)
					== SOAP_OK)) { /* depth 1*/
				write_true(writer);

				//push SensorConfig: next 21

			} else {
				label = 21; //TODO: xx
				write_false(writer);
				continue;
			} /* SensorConfig */

			break;
		} /* stateEnd */

		case 2: //stateComplexBegin(Acceleration)
		{
			label = 3; //assert 3==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:Acceleration", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push Acceleration: next 5

			} else {
				label = 5; //TODO: xx
				write_false(writer);
				continue;
			} /* Acceleration */

			break;
		} /* stateEnd */

		case 3: //stateBegin(rate)
		{
			label = 4; //assert 4==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 4: //stateComplexEnd(Acceleration) : 0..1
		{

			label = 2; /* constLoopEnd1 /StatusControl/SensorConfig/Acceleration/ */

			if (soap_element_end_in(soap, "sens:Acceleration") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: Acceleration expected",
						NULL);
				return -1;
			}

//stateNameEnd Acceleration

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Acceleration/ */

		} /* case */
			//pop Acceleration

		case 5: //stateComplexBegin(Audio)
		{
			label = 6; //assert 6==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:Audio", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push Audio: next 8

			} else {
				label = 8; //TODO: xx
				write_false(writer);
				continue;
			} /* Audio */

			break;
		} /* stateEnd */

		case 6: //stateBegin(rate)
		{
			label = 7; //assert 7==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 7: //stateComplexEnd(Audio) : 0..1
		{

			label = 5; /* constLoopEnd1 /StatusControl/SensorConfig/Audio/ */

			if (soap_element_end_in(soap, "sens:Audio") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: Audio expected", NULL);
				return -1;
			}

//stateNameEnd Audio

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Audio/ */

		} /* case */
			//pop Audio

		case 8: //stateComplexBegin(Light)
		{
			label = 9; //assert 9==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:Light", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push Light: next 11

			} else {
				label = 11; //TODO: xx
				write_false(writer);
				continue;
			} /* Light */

			break;
		} /* stateEnd */

		case 9: //stateBegin(rate)
		{
			label = 10; //assert 10==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 10: //stateComplexEnd(Light) : 0..1
		{

			label = 8; /* constLoopEnd1 /StatusControl/SensorConfig/Light/ */

			if (soap_element_end_in(soap, "sens:Light") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: Light expected", NULL);
				return -1;
			}

//stateNameEnd Light

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Light/ */

		} /* case */
			//pop Light

		case 11: //stateComplexBegin(AmbientLight)
		{
			label = 12; //assert 12==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:AmbientLight", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push AmbientLight: next 14

			} else {
				label = 14; //TODO: xx
				write_false(writer);
				continue;
			} /* AmbientLight */

			break;
		} /* stateEnd */

		case 12: //stateBegin(rate)
		{
			label = 13; //assert 13==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 13: //stateComplexEnd(AmbientLight) : 0..1
		{

			label = 11; /* constLoopEnd1 /StatusControl/SensorConfig/AmbientLight/ */

			if (soap_element_end_in(soap, "sens:AmbientLight") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: AmbientLight expected",
						NULL);
				return -1;
			}

//stateNameEnd AmbientLight

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/AmbientLight/ */

		} /* case */
			//pop AmbientLight

		case 14: //stateComplexBegin(Force)
		{
			label = 15; //assert 15==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:Force", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push Force: next 17

			} else {
				label = 17; //TODO: xx
				write_false(writer);
				continue;
			} /* Force */

			break;
		} /* stateEnd */

		case 15: //stateBegin(rate)
		{
			label = 16; //assert 16==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 16: //stateComplexEnd(Force) : 0..1
		{

			label = 14; /* constLoopEnd1 /StatusControl/SensorConfig/Force/ */

			if (soap_element_end_in(soap, "sens:Force") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: Force expected", NULL);
				return -1;
			}

//stateNameEnd Force

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Force/ */

		} /* case */
			//pop Force

		case 17: //stateComplexBegin(Temperature)
		{
			label = 18; //assert 18==label+1

			/* lowerbound=0 upperbound=1*/

			if ((soap_element_begin_in(soap, "sens:Temperature", 0, NULL)
					== SOAP_OK)) { /* depth 2*/
				write_true(writer);

				//push Temperature: next 20

			} else {
				label = 20; //TODO: xx
				write_false(writer);
				continue;
			} /* Temperature */

			break;
		} /* stateEnd */

		case 18: //stateBegin(rate)
		{
			label = 19; //assert 19==label+1

			if (!(soap_element_begin_in(soap, "sens:rate", 0, NULL)
					== SOAP_OK)) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: rate expected", NULL);
				// soap->error=SOAP_TAG_MISMATCH;
				return -1;
			}

			{

				{
					ssize_t ret = 0;

					{
						int8_t c;
						const char* str = soap_value(soap);
						if (sscanf(str, "%hhd", &c) != 1) {
							fprintf(stderr, "Error parsing char value\n");
							fprintf(stderr, "Input string: %s", str);
							ret = -1;
						}

						{
							uint8_t d = ((c) - (-128)) / 1;
							ret = write_bits(writer, (u_char *) &d, 8);
						}

					}

				}

				if (soap_element_end_in(soap, "sens:rate") != SOAP_OK) {
					soap->error = soap_sender_fault(soap,
							"tag name or namespace mismatch: rate expected",
							NULL);
					return -1;
				}

//stateNameEnd rate

			}

			break;
		} /* stateEnd */

		case 19: //stateComplexEnd(Temperature) : 0..1
		{

			label = 17; /* constLoopEnd1 /StatusControl/SensorConfig/Temperature/ */

			if (soap_element_end_in(soap, "sens:Temperature") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: Temperature expected",
						NULL);
				return -1;
			}

//stateNameEnd Temperature

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/Temperature/ */

		} /* case */
			//pop Temperature

		case 20: //stateComplexEnd(SensorConfig) : 0..1
		{

			label = 1; /* constLoopEnd1 /StatusControl/SensorConfig/ */

			if (soap_element_end_in(soap, "sens:SensorConfig") != SOAP_OK) {
				soap->error = soap_sender_fault(soap,
						"tag name or namespace mismatch: SensorConfig expected",
						NULL);
				return -1;
			}

//stateNameEnd SensorConfig

			continue; /* constLoopEnd2 /StatusControl/SensorConfig/ */

		} /* case */
			//pop SensorConfig

		case 21: //stateBegin(NewTime)
		{
			label = 22; //assert 22==label+1

			/* loop /StatusControl/NewTime */
			while ((soap_element_begin_in(soap, "sens:NewTime", 0, NULL)
					== SOAP_OK)) {
				write_true(writer);

				{

					{
						ssize_t ret = 0;

						{
							struct timeval t;
							const char *str = soap_value(soap);

							ret = soap_s2dateTime(soap, str, &(t.tv_sec));
							if (ret != 0) {
								fprintf(stderr, "Error parsing time value\n");
								fprintf(stderr, "Input string: %s", str);
								ret = -1;
							} else {

								{
									ret = write_bits(writer,
											(u_char *) &(t).tv_sec, 32);

									{
										uint16_t usec;
										usec = (t).tv_usec >> 4;
										ret += write_bits(writer,
												(u_char *) &usec, 12);
									}

								};
							}
						}

					}

					if (soap_element_end_in(soap, "sens:NewTime") != SOAP_OK) {
						soap->error =
								soap_sender_fault(
										soap,
										"tag name or namespace mismatch: NewTime expected",
										NULL);
						return -1;
					}

//stateNameEnd NewTime

				} /* NewTime */

			}
			write_false(writer); //XXX: not necessary, right now symetric to bin2sax...

			break;
		} /* stateEnd */

		case 22: //stateComplexEnd(StatusControl) : 1..1
		{

			label = 23; // Complex End

			if (soap_element_end_in(soap, "sens:StatusControl") != SOAP_OK) {
				soap->error =
						soap_sender_fault(
								soap,
								"tag name or namespace mismatch: StatusControl expected",
								NULL);
				return -1;
			}

//stateNameEnd StatusControl

			break;

		} /* case */
			//pop StatusControl

		default: //StopState

			return 0;

		}
	}

	return 1;
}

