/*
 * main.c
 *
 *  Created on: Nov 24, 2010
 *      Author: Daniel Wildschut <wildschu@teco.edu>
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
/*#include "config.h"
#include "errors.h"
#include "serial.h"
#include "base_station.h"
#include "node.h"*/

int main(int argc, char **argv)
{
	char *addr = NULL;
	unsigned short nodeid = 0;
	unsigned short channel = 1;

	while (argc > 1) {
			if (argv[1][0] == '-') {
				char *option = &argv[1][1];

				switch (option[0]) {
				case 'p':
					if (strlen(option) > 2) {
						++option;
						addr = option;
					} else {
						--argc;
						++argv;
						addr = argv[1];
					}

					break;

				case 'n':
					if (strlen(option) > 2) {
						++option;
						nodeid = strtoul(option,NULL,10);
					} else {
						--argc;
						++argv;
						nodeid = strtoul(argv[1],NULL,10);
					}

					break;

				case 'c':
					if (strlen(option) > 2) {
						++option;
						channel = strtoul(option,NULL,10);
					} else {
						--argc;
						++argv;
						channel = strtoul(argv[1],NULL,10);
					}

					break;

				default:
					fprintf(stderr, "MicroStrainTest: Bad option %s\n", argv[1]);
					printf("\n%s -p [COM Port device] -n [Node ID] -c [Sensor channel]\n",
						argv[0]);
					exit(1);
				}
			}
			--argc;
			++argv;
		}

	signal(SIGIO, SIG_IGN);

	printf("Opening connection to base station at %s\n",addr);
	int handle = open(addr,O_RDWR|O_NOCTTY| O_NONBLOCK| O_SYNC);
	if(handle==-1)
	{
		printf("Opening connection failed\n");
		perror(NULL);
		exit(1);
	}
	fcntl (handle, F_SETFL, O_ASYNC);
	printf("Setting options\n");
	struct termios options;
	tcgetattr(handle, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 1;

	tcsetattr(handle, TCSAFLUSH, &options);
	printf("Trying to ping basestation\n");

	unsigned char	ucCmd = 0x01,	//	command to send
						ucResponse;		//	holds the response
	/*while(read(handle,&ucResponse,1)!=-1)
	{
		printf("Clearing buffer: %d\n",ucResponse);
	}*/
	printf("Writing... %d\n",ucCmd);
	int sent = write(handle,&ucCmd,1);
	if(sent==-1)
	{
		printf("Writing ping command failed\n");
		perror(NULL);
		exit(1);
	}
	printf("Reading...\n");
	int received = read(handle,&ucResponse,1);
	if(received==-1)
	{
		printf("Reading ping response failed\n");
		perror(NULL);
		exit(1);
	}
	if(ucResponse!=0x01)
	{
		printf("Wrong response %d received\n",ucResponse);
	} else {
		printf("Ping successful\n",ucResponse);
	}


	printf("Trying to ping node %d\n",nodeid);
	unsigned char	ucNodePing[3] =	{	0x02,	//	ping command byte
										nodeid >> 8,	//	MSB of the node address
										nodeid & 0xFF	//	LSB of the node address
									};
	printf("Writing... %d %d %d\n",ucNodePing[0],ucNodePing[1],ucNodePing[2]);
	sent = write(handle,&ucNodePing,3);
	if(sent==-1)
	{
		printf("Writing ping command failed\n");
		perror(NULL);
		exit(1);
	}
	printf("Reading...\n");
	received = read(handle,&ucResponse,1);
	if(received==-1)
	{
		printf("Reading ping response failed\n");
		perror(NULL);
		exit(1);
	}
	if(ucResponse!=0x02)
	{
		printf("Wrong response %d received\n",ucResponse);
	} else {
		printf("Ping successful\n");
	}

	printf("Reading sensor values\n");
	printf("Reading sensor channel %d\n",channel);
	unsigned char ucNodeRead[5] =	{	0x03,	//	read single sensor command byte
											nodeid >> 8,	//	MSB of the node address
											nodeid & 0xFF,	//	LSB of the node address,
											0x01,
											channel
									};
	unsigned char ucReadResponse[5];
	printf("Writing... %d %d %d %d %d\n",ucNodeRead[0],ucNodeRead[1],ucNodeRead[2],ucNodeRead[3],ucNodeRead[4]);
		sent = write(handle,&ucNodeRead,5);
		if(sent==-1)
		{
			printf("Writing read command failed\n");
			perror(NULL);
			exit(1);
		}
		printf("Reading...\n");
		received = read(handle,&ucReadResponse,5);
		if(received==-1)
		{
			printf("Reading read response failed\n");
			perror(NULL);
			exit(1);
		}
		if(ucReadResponse[0]==0x21)
		{
			printf("Wrong response received\n");
		} else {
			printf("Read successful\n");
			float val = ((ucReadResponse[2] << 8) + ucReadResponse[3])/2;
			printf("Read value: %f\n",val);
		}

	printf("Closing connection\n");
	close(handle);
}
