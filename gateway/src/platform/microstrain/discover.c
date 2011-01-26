#include <ws4d-gSOAP/dpws_device.h>
#include <assert.h>

#include "../../discover.h"
#include "../../proxy_handler.h"
#include "../../proxy_structures.h"
#include "../../service_cache.h"

#ifndef WIN32
#include <pthread.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "microstrain.h"

#ifdef WIN32
DWORD workerid;
HANDLE discovery_worker;
#else
pthread_t discovery_worker;
#endif

volatile int shutdownFlag_discovery = 0; /* Flag indicates systemshutdown. */

static int has_NodeId(void *c,void *id) {return (0==memcmp(&((msdevice *)c)->id,id,sizeof(unsigned short)));}
#define device_proxy_list_get_device_by_id(id) device_proxy_list_get_device(has_NodeId,id)

/**
 * Serial handle
 */
int handle = -1;
char * device = "/dev/com3";

/**
 * Message queue
 */

#define QUEUELENGTH 10
msmessage msgqueue[10];

pthread_mutex_t receivedmutex = PTHREAD_MUTEX_INITIALIZER;

void signalmessage()
{

}

int openSerial()
{

	/*struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signalmessage;
	sigaction(SIGIO, &sa, NULL);*/

	signal(SIGIO, SIG_IGN);


	handle = open(device,O_RDWR|O_NOCTTY| O_SYNC);
	if(handle==-1)
	{
		printf("Unable to open connection to %s\n",device);
		perror(NULL);
		return 0;
	}
	//fcntl (handle, F_SETFL, O_ASYNC);
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
	options.c_cc[VTIME] = 10;

	tcsetattr(handle, TCSAFLUSH, &options);

	// Ping base station
	unsigned char ucCmd = 0x01;
	unsigned char ucResponse;
	int bytes = write(handle,&ucCmd,1);
	if(bytes==-1)
	{
		perror(NULL);
		printf("Unable to ping base station\n");
		return 0;
	}

	bytes = read(handle,&ucResponse,1);
	if(bytes==-1)
	{
		perror(NULL);
		return 0;
	}
	if(ucResponse!=0x01)
	{
		printf("Pinging base station failed\n");
		return 0;
	}

	memset(msgqueue,0,sizeof(msmessage)*QUEUELENGTH);
	int i = 0;
	for(i=0;i<QUEUELENGTH;i++)
	{
		msgqueue[i].type = MS_NONE;
	}

	return 1;
}

int closeSerial()
{
	int ret = close(handle);
	if(ret==-1)
	{
		perror(NULL);
		return 0;
	}
	return 1;
}

int inMessageQueue(int type)
{
	int i = 0;
	for(i=0;i<QUEUELENGTH;i++)
	{
		if(msgqueue[i].type == type)
		{
			return 1;
		}
	}
	return 0;
}

int getMessageFromQueue(int type, msmessage * dest)
{
	int i = 0;
	for(i=0;i<QUEUELENGTH;i++)
	{
		if(msgqueue[i].type == type)
		{
			memcpy(dest,&msgqueue[i],sizeof(msmessage));
			msgqueue[i].type = MS_NONE;
			return 1;
		}
	}

	return 0;
}

int readMessage(int expectmultiple)
{
	pthread_mutex_lock(&receivedmutex);
	int msgslot = 0;
	int bytes_read = 0;
	time_t cursecs = time(NULL);

	// Remove stale messages
	for(msgslot=0;msgslot<QUEUELENGTH;msgslot++)
	{
		if(cursecs-msgqueue[msgslot].received.tv_sec>30&&msgqueue[msgslot].type!=MS_NONE)
		{
			printf("Removed stale message %s in slot %d\n",msmessagestr(msgqueue[msgslot].type),msgslot);
			msgqueue[msgslot].type=MS_NONE;
		}
	}

	// Search for available message
	for(msgslot=0;msgslot<QUEUELENGTH;msgslot++)
	{
		if(msgqueue[msgslot].type==MS_NONE)
		{
			break;
		}
	}

	if(msgslot==QUEUELENGTH)
	{
		printf("No open slot\n");
		pthread_mutex_unlock(&receivedmutex);
		return -1;
	}

	memset(msgqueue[msgslot].buffer,0,sizeof(unsigned char)*266);

	// Read a byte and determine package type
	bytes_read = read(handle,msgqueue[msgslot].buffer,1);

	if(bytes_read==-1)
	{
		pthread_mutex_unlock(&receivedmutex);
		perror(NULL);
		return -1;
	}

	if(bytes_read!=1)
	{
		pthread_mutex_unlock(&receivedmutex);
		return 0;
	}

	//printf("Received byte: %#x\n",msgqueue[msgslot].buffer[0]);
	gettimeofday(&msgqueue[msgslot].received,NULL);

	switch(msgqueue[msgslot].buffer[0])
	{

	case 0x02:
	{
		msgqueue[msgslot].type = MS_SHORTPING;
		msgqueue[msgslot].length = 0;
	} break;

	case 0x03:
	{
		memset(msgqueue[msgslot].buffer,0,sizeof(unsigned char)*266);
		bytes_read = read(handle,msgqueue[msgslot].buffer,4);
		if(bytes_read==-1)
		{
			perror(NULL);
			pthread_mutex_unlock(&receivedmutex);
			return -1;
		}
		if(bytes_read!=4)
		{
			printf("Failed to read rest of MS_READVALUE message\n");
			pthread_mutex_unlock(&receivedmutex);
			return -1;
		}
		msgqueue[msgslot].type = MS_READVALUE;
		msgqueue[msgslot].length = 4;


	} break;

	case 0x04:
	{
		msgqueue[msgslot].type = MS_WRITEEEPROM;
		msgqueue[msgslot].length = 0;
	} break;

	case 0x05:
	{
		memset(msgqueue[msgslot].buffer,0,sizeof(unsigned char)*266);
		bytes_read = read(handle,msgqueue[msgslot].buffer,266);
		if(bytes_read==-1)
		{
			perror(NULL);
			pthread_mutex_unlock(&receivedmutex);
			return -1;
		}

		if(bytes_read!=266)
		{
			printf("Failed to read rest of MS_DATAPAGE message\n");
			pthread_mutex_unlock(&receivedmutex);
			return -1;
		}
		msgqueue[msgslot].length = 266;
		msgqueue[msgslot].type = MS_DATAPAGE;

	} break;

	case 0x06:
	{
		msgqueue[msgslot].type = MS_ERASE;
		msgqueue[msgslot].length = 0;
	} break;

	case 0x0C:
	{
		msgqueue[msgslot].type = MS_TRIGGERLOGGING;
		msgqueue[msgslot].length = 0;
	} break;

	case 0x21:
	{
		msgqueue[msgslot].type = MS_FAILURE;
		msgqueue[msgslot].length = 0;
	} break;

	case 0x90:
	{
		msgqueue[msgslot].type = MS_STOP;
		msgqueue[msgslot].length = 0;
	} break;

	case 0xAA:
	{
		if(expectmultiple)
		{
			memset(msgqueue[msgslot].buffer,0,sizeof(unsigned char)*266);
			bytes_read = read(handle,msgqueue[msgslot].buffer,2);
			if(bytes_read==-1)
			{
				perror(NULL);
				printf("Failed to read rest of message\n");
				pthread_mutex_unlock(&receivedmutex);
				return -1;
			}
			if(bytes_read==0)
			{
				msgqueue[msgslot].type = MS_BS;
				msgqueue[msgslot].length = 0;
				break;
			}
			if(bytes_read!=2)
			{
				printf("Only read %d bytes\n",bytes_read);
			}

			if(msgqueue[msgslot].buffer[0]==0x07 && (msgqueue[msgslot].buffer[1]==0x02))
			{
				bytes_read = read(handle,msgqueue[msgslot].buffer,9);
				if(bytes_read==-1)
				{
					perror(NULL);
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}

				if(bytes_read!=9)
				{
					printf("Failed to read rest of MS_LONGPING message\n");
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}
				msgqueue[msgslot].length = 9;
				msgqueue[msgslot].type = MS_LONGPING;
			} else if(msgqueue[msgslot].buffer[0]==0x07 && msgqueue[msgslot].buffer[1]==0x04)
			{
				bytes_read = read(handle,msgqueue[msgslot].buffer,9);
				if(bytes_read==-1)
				{
					perror(NULL);
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}

				if(bytes_read!=9)
				{
					printf("Failed to read rest of MS_LDC message, only received %d bytes\n",bytes_read);
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}
				msgqueue[msgslot].length = 9;
				// Compute length of LDC message
				int i = 0;
				int activechannels = 0;
				for (i = 0; i < 8; i++)
				{
					if ((msgqueue[msgslot].buffer[4] >> i) & 1) {
						activechannels++;
					}
				}
				int remainingsize = 2 * activechannels + 4;

				bytes_read = read(handle,&msgqueue[msgslot].buffer[9],remainingsize);
				if(bytes_read==-1)
				{
					perror(NULL);
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}

				if(bytes_read!=remainingsize)
				{
					printf("Failed to read rest of MS_LDC message\n");
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}
				msgqueue[msgslot].length += remainingsize;

				msgqueue[msgslot].type = MS_LDC;

			} else if(msgqueue[msgslot].buffer[0]==0x01 && msgqueue[msgslot].buffer[1]==0x05)
			{
				printf("High speed streaming is currently not supported\n");
				pthread_mutex_unlock(&receivedmutex);
				return -1;
				// TODO: Deal with High speed streaming packets, length may be greater than 266
			} else if(((msgqueue[msgslot].buffer[0] & 0x08) && msgqueue[msgslot].buffer[1]==0x07))
			{
				bytes_read = read(handle,msgqueue[msgslot].buffer,10);
				if(bytes_read==-1)
				{
					perror(NULL);
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}

				if(bytes_read!=10)
				{
					printf("Failed to read rest of MS_DISCOVERY message\n");
					pthread_mutex_unlock(&receivedmutex);
					return -1;
				}
				msgqueue[msgslot].length = 10;
				msgqueue[msgslot].type = MS_DISCOVERY;

			} else {
				// Got the start of the next packet
				printf("Got start of next packet\n");
				printf("Got bytes %#x %#x\n",msgqueue[msgslot].buffer[0],msgqueue[msgslot].buffer[1]);
				msgqueue[msgslot].type = MS_BS;
				msgqueue[msgslot].length = 0;
			}
		} else {
			msgqueue[msgslot].type = MS_BS;
			msgqueue[msgslot].length = 0;
		}

	}break;

	case 0xFF:
	{
		printf("Real time streaming is currently not supported\n");
		// TODO: Deal with real time streaming missing node identifier and channel mask
		pthread_mutex_unlock(&receivedmutex);
		return -1;

	}break;

	default:
	{
		printf("Unknown message type\n");
		pthread_mutex_unlock(&receivedmutex);
		return -1;
	}

	}

	//printf("%s message received\n",msmessagestr(msgqueue[msgslot].type));
	pthread_mutex_unlock(&receivedmutex);
	return 1;

}

void processDiscovery(msmessage * msg)
{
	unsigned short id = (msg->buffer[0] << 8) + msg->buffer[1];
	if(device_proxy_list_get_device_by_id(&id)!=NULL)
	{
		return;
	}
	registerNode(id);
	initLDC(id);
}

void registerNode(unsigned short id)
{
	char * friendly = NULL;
	char * model = NULL;
	char * serial_num = NULL;
	char * version = NULL;

	msdevice dev;
	memset(&dev,0,sizeof(msdevice));
	dev.id = id;
	gettimeofday(&dev.updated,NULL);
	updateNodeInfo(&dev);

	// Get version
	requestEEPROM(id,108);
	readMessage(0);
	/*if(ret==-1)
	{
		printf("Failed to read node fw version\n");
	}*/
	while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}
	msmessage versionmsg;
	if(inMessageQueue(MS_READVALUE))
	{
		getMessageFromQueue(MS_READVALUE,&versionmsg);
		asprintf(&version,"%d.%d",versionmsg.buffer[0],versionmsg.buffer[1]);
	} else if(inMessageQueue(MS_FAILURE)){
		asprintf(&version,"1");
	}


	switch(dev.modelnumber)
	{
	case 2513:
	{
		friendly = "MicroStrain V-Link";
		model = "2513";
	}break;

	case 2413:
	{
		friendly = "MicroStrain SG-Link";
		model = "2413";
	}break;

	case 2314:
	{
		friendly = "MicroStrain G-Link 10G";
		model = "2314";
	}break;

	case 2313:
	{
		friendly = "MicroStrain G-Link 2G";
		model = "2313";
	}break;
	}

	asprintf(&serial_num,"%s-%d",model,id);
	dpws_device_build_and_register(model,&dev,sizeof(msdevice), gateway_get_interface(),version, serial_num, friendly);
	printf("New %s node %d found\n",friendly,id);
	if(version!=NULL)
	{
		free(version);
	}
	free(serial_num);
}

void updateNodeInfo(msdevice * node)
{
	msmessage msg;

	// Get Model number
	requestEEPROM(node->id,112);
	readMessage(0);
	/*if(ret==-1)
	{
		printf("Failed to read node model\n");
	}*/
	while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_READVALUE))
	{
		getMessageFromQueue(MS_READVALUE,&msg);
		node->modelnumber = (msg.buffer[0] << 8) + msg.buffer[1];
		printf("Received model number: %d\n",(msg.buffer[0] << 8) + msg.buffer[1]);
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		// Set to G-Link model number
		node->modelnumber = 2314;
	}

	// Get channel mask
	requestEEPROM(node->id,12);
	readMessage(0);
	/*if(ret==-1)
	{
		printf("Failed to read node channel mask\n");
	}*/
	while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_READVALUE))
	{
		getMessageFromQueue(MS_READVALUE,&msg);
		node->channelmask = (msg.buffer[0] << 8) + msg.buffer[1];
		printf("Received channel mask: %d\n",(msg.buffer[0] << 8) + msg.buffer[1]);
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		node->channelmask = 8;
	}

	// Get LDC rate
	requestEEPROM(node->id,72);
	readMessage(0);

	while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_READVALUE))
	{
		getMessageFromQueue(MS_READVALUE,&msg);
		node->ldcrate = getLDCRate((msg.buffer[0] << 8) + msg.buffer[1]);
		printf("Received LDC frequency: Index %d, Frequency %f\n",(msg.buffer[0] << 8) + msg.buffer[1],getLDCRate((msg.buffer[0] << 8) + msg.buffer[1]));
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		node->ldcrate = 0.2f;
	}

	// Get channel actions
	int i = 0;
	for(i=0;i<8;i++)
	{
		requestEEPROM(node->id,150+10*i);
		readMessage(0);
		/*if(ret==-1)
		{
			printf("Failed to read node channel action %d ID\n",i);
		}*/
		while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
		{
			readMessage(0);
		}

		if(inMessageQueue(MS_READVALUE))
		{
			getMessageFromQueue(MS_READVALUE,&msg);
			node->channelaction[i] = msg.buffer[0];
			printf("Received channel action %d id %d\n",i,msg.buffer[0]);
		} else if(inMessageQueue(MS_FAILURE))
		{
			getMessageFromQueue(MS_FAILURE,&msg);
			node->channelaction[i] = 0x04;
		}

		// Get gain


		node->gain[i] = 1;
		unsigned char * memlocation = (unsigned char *)&node->gain[i];

		requestEEPROM(node->id,152+10*i);
		readMessage(0);
		/*if(ret==-1)
		{
			printf("Failed to read node channel action %d ID\n",i);
		}*/
		while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
		{
			readMessage(0);
		}

		if(inMessageQueue(MS_READVALUE))
		{
			getMessageFromQueue(MS_READVALUE,&msg);
			memlocation[0] = msg.buffer[0];
			memlocation[1] = msg.buffer[1];
		} else if(inMessageQueue(MS_FAILURE))
		{
			getMessageFromQueue(MS_FAILURE,&msg);
		}

		requestEEPROM(node->id,154+10*i);
		readMessage(0);
		/*if(ret==-1)
		{
			printf("Failed to read node channel action %d ID\n",i);
		}*/
		while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
		{
			readMessage(0);
		}

		if(inMessageQueue(MS_READVALUE))
		{
			getMessageFromQueue(MS_READVALUE,&msg);
			memlocation[2] = msg.buffer[0];
			memlocation[3] = msg.buffer[1];
		} else if(inMessageQueue(MS_FAILURE))
		{
			getMessageFromQueue(MS_FAILURE,&msg);
		}
		printf("Received channel action %d gain %f\n",i,node->gain[i]);
		// Get offset

		node->offset[i] = 0;
		memlocation = (unsigned char *)&node->offset[i];

		requestEEPROM(node->id,156+10*i);
		readMessage(0);
		/*if(ret==-1)
		{
			printf("Failed to read node channel action %d ID\n",i);
		}*/
		while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
		{
			readMessage(0);
		}

		if(inMessageQueue(MS_READVALUE))
		{
			getMessageFromQueue(MS_READVALUE,&msg);
			memlocation[0] = msg.buffer[0];
			memlocation[1] = msg.buffer[1];
		} else if(inMessageQueue(MS_FAILURE))
		{
			getMessageFromQueue(MS_FAILURE,&msg);
		}

		requestEEPROM(node->id,158+10*i);
		readMessage(0);
		/*if(ret==-1)
		{
			printf("Failed to read node channel action %d ID\n",i);
		}*/
		while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
		{
			readMessage(0);
		}

		if(inMessageQueue(MS_READVALUE))
		{
			getMessageFromQueue(MS_READVALUE,&msg);
			memlocation[2] = msg.buffer[0];
			memlocation[3] = msg.buffer[1];
		} else if(inMessageQueue(MS_FAILURE))
		{
			getMessageFromQueue(MS_FAILURE,&msg);
		}
		printf("Received channel action %d offset %f\n",i,node->offset[i]);
	}
}

int pingNode(unsigned short id)
{
	unsigned char	pingCmd[3] =	{	0x02,	//	ping command byte
			id >> 8,	//	MSB of the node address
			id & 0xFF	//	LSB of the node address
	};
	msmessage msg;

	int bytes = write(handle,pingCmd,3);
	if(bytes!=3)
	{
		printf("Written only %d instead of 3 bytes\n",bytes);
		return 0;
	}
	readMessage(0);
	while(!inMessageQueue(MS_SHORTPING)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_SHORTPING))
	{
		getMessageFromQueue(MS_SHORTPING,&msg);
		return 1;
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
	}

	return 0;
}

void processLDC(msmessage * msg)
{
	printf("Processing LDC\n");
	unsigned short id = (msg->buffer[0] << 8) + msg->buffer[1];
	struct remote_device *rem_device = device_proxy_list_get_device_by_id(&id);
	struct dpws_s *device = rem_device ? remote_device_get_dpws_device(	rem_device) : NULL;
	if (!device) {
		printf("device for event not found");
		return;
	}
	int checksum = 0x07 + 0x04;
	int i = 0;
	for(i = 0;i< msg->length-4;i++)
	{
		checksum+=msg->buffer[i];
	}
	if(checksum!=((msg->buffer[msg->length-2] << 8) + msg->buffer[msg->length-1]))
	{
		printf("LDC Checksum failed. Should be %d, is %d\n",checksum,(msg->buffer[msg->length-2] << 8) + msg->buffer[msg->length-1]);
		return;
	}
	msdevice * msdev = ((msdevice*) remote_device_get_addr(rem_device));
	gettimeofday(&msdev->updated,NULL);

	//printf("Timertick: %d\n",((msg->buffer[7] << 8) + msg->buffer[8]));
	// Delta is timertick * LDC rate (EEPROM 72)
	unsigned short tick = ((msg->buffer[7] << 8) + msg->buffer[8]);
	float delta = 0;
	if(msdev->lasttick >= delta)
	{
		delta = (tick - msdev->lasttick);
	} else {
		// Wrap around
		delta = (USHRT_MAX + tick - msdev->lasttick);
	}
	msdev->lasttick = tick;
	delta*=msdev->ldcrate;
	//printf("Delta: %f Rate: %f\n",delta,msdev->ldcrate);

	for (i = 0; i < 8; i++)
	{
		if ((msg->buffer[4] >> i) & 1) {
			msdev->lastvalue[i] = ((msg->buffer[9+2*i] << 8) + msg->buffer[10+2*i])>>1;
			//printf("Read raw value: %f\n",msdev->lastvalue[i]);
			// Apply channel action
			switch(msdev->channelaction[i])
			{
			case 0x01:
			{
				// Strain
				msdev->lastvalue[i] = msdev->gain[i] * (msdev->lastvalue[i] + msdev->offset[i]);
			} break;

			case 0x02:
			{
				// G-Link
				if(msdev->gain[i]!=0)
				{
					msdev->lastvalue[i] =  (msdev->lastvalue[i] - msdev->offset[i]) / msdev->gain[i];
					//printf("Read G-Link value: %f\n",msdev->lastvalue[i]);
				}
			} break;

			case 0x04:
			{
				// Custom formula
				msdev->lastvalue[i] =  msdev->lastvalue[i] * msdev->gain[i] + msdev->offset[i] ;
			} break;
			}
		} else {
			msdev->lastvalue[i] = 0;
		}
	}
	printf("Delivering LDC event\n");
	float buffer[10];
	memset(buffer,0,sizeof(float)*0);
	memcpy(buffer,msdev->lastvalue,sizeof(float)*8);
	buffer[8] = tick;
	buffer[9] = delta;
	AccelModel_event(0,0,device,(char*)buffer,10*sizeof(float));

}

void requestEEPROM(unsigned short node,unsigned short eepromaddr)
{
	unsigned char	cmd[5] =	{	0x03,	//	eeprom read command byte
			node >> 8,	//	MSB of the node address
			node & 0xFF,	//	LSB of the node address
			eepromaddr >> 8,	//	MSB of the location to read
			eepromaddr & 0xFF	//	LSB of the location to read
	};

	int bytes = write(handle,cmd,5);
	if(bytes==-1)
	{
		printf("Error requesting EEPROM data\n");
		perror(NULL);
	} else if(bytes!=5)
	{
		printf("Unable to request EEPROM data\n");
	}
}

void initLDC(unsigned short node)
{
	// Set streaming duration to infinite
	unsigned char	eepromcmd[8] =	{	0x04,			//	eeprom write command byte
			node >> 8,	//	MSB of the node address
			node & 0xFF,	//	LSB of the node address
			100,		//	location to write
			1 >> 8,	//	MSB of value to write
			1 & 0xFF,	//	LSB of value to write
			0x00,			//	MSB of the checksum
			0x00			//	LSB of the checksum
	};
	/*	calculate the checksum	*/
	unsigned short checksum = (eepromcmd[1] + eepromcmd[2] + eepromcmd[3] + eepromcmd[4] + eepromcmd[5]) & 0xFFFF;
	eepromcmd[6] = checksum >> 8;		//	set the MSB of the checksum
	eepromcmd[7] = checksum & 0xFF;	//	set the LSB of the checksum

	int bytes = write(handle,eepromcmd,8);

	if(bytes==-1)
	{
		printf("Error writing node EEPROM\n");
		perror(NULL);
	} else if(bytes!=8)
	{
		printf("Unable to write node EEPROM, wrote only %d\n",bytes);
	}

	msmessage msg;
	readMessage(0);
	while(!inMessageQueue(MS_WRITEEEPROM)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_WRITEEEPROM))
	{
		getMessageFromQueue(MS_WRITEEEPROM,&msg);
		printf("Wrote node EEPROM\n");
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		printf("Failed to write node EEPROM\n");
	}

	printf("Starting LDC\n");
	// Start LDC
	unsigned char ldccmd[10] = {	0xAA,			//	SOP
			0x05,			//	Message Type
			0x00,			//	Address Mode
			node >> 8,	//	MSB of Node Address
			node & 0xFF,	//	LSB of Node Address
			0x02,			//	Command Length
			0x00,			//	MSB of Command Byte
			0x38,			//	LSB of Command Byte
			(0x3F + (node >> 8) + (node & 0xFF)) >> 8,		//	MSB of the checksum
			(0x3F + (node >> 8) + (node & 0xFF)) & 0xFF		//	LSB of the checksum
	};
	bytes = write(handle,ldccmd,10);
	if(bytes==-1)
	{
		printf("Error starting LDC\n");
		perror(NULL);
	} else if(bytes!=10)
	{
		printf("Unable to start LDC\n");
	}

	readMessage(0);
	while(!inMessageQueue(MS_BS))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_BS))
	{
		getMessageFromQueue(MS_BS,&msg);
		printf("Started node %d in LDC\n",node);
	}
}

int stopNode(unsigned short node)
{
	unsigned char cmd[10] = {	0xAA,			//	SOP
			0xFE,			//	Message Type
			0x00,			//	Address Mode
			node >> 8,	//	MSB of Node Address
			node & 0xFF,	//	LSB of Node Address
			0x02,			//	Command Length
			0x00,			//	MSB of Command Byte
			0x90,			//	LSB of Command Byte
			(0x190 + (node >> 8) + (node & 0xFF)) >> 8,		//	MSB of the checksum
			(0x190 + (node >> 8) + (node & 0xFF)) & 0xFF	//	LSB of the checksum
	};
	tcflush(handle,TCIOFLUSH);
	int bytes = write(handle,cmd,10);
	if(bytes==-1)
	{
		printf("Error stopping node\n");
		perror(NULL);
		return 0;
	} else if(bytes!=10)
	{
		printf("Unable to stop node\n");
		return 0;
	}

	msmessage msg;
	readMessage(0);
	while(!inMessageQueue(MS_BS))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_BS))
	{
		getMessageFromQueue(MS_BS,&msg);
	}

	readMessage(0);
	while(!inMessageQueue(MS_STOP)&&inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_STOP))
	{
		getMessageFromQueue(MS_STOP,&msg);
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		return 0;
	}

	return 1;
}

int stopDevice(void *in,void *dummy)
{
	msdevice * msdev=(msdevice*) in;
	stopNode(msdev->id);
	sleep(1);
	return 0;
}

int initLDCDevice(void *in,void *dummy)
{
	msdevice * msdev=(msdevice*) in;
	initLDC(msdev->id);
	return 0;
}

char * msmessagestr(int type)
		{
	switch(type)
	{
	case MS_STREAM:
	{
		return "MS_STREAM";
	}break;

	case MS_HSS:
	{
		return "MS_HSS";
	}break;

	case MS_LDC:
	{
		return "MS_LDC";
	}break;

	case MS_LONGPING:
	{
		return "MS_LONGPING";
	}break;

	case MS_STOP:
	{
		return "MS_STOP";
	}break;

	case MS_SHORTPING:
	{
		return "MS_SHORTPING";
	}break;
	case MS_READVALUE:
	{
		return "MS_READVALUE";
	}break;
	case MS_DATAPAGE:
	{
		return "MS_DATAPAGE";
	}break;
	case MS_WRITEEEPROM:
	{
		return "MS_WRITEEEPROM";
	}break;
	case MS_ERASE:
	{
		return "MS_ERASE";
	}break;
	case MS_TRIGGERLOGGING:
	{
		return "MS_TRIGGERLOGGING";
	}break;
	case MS_DISCOVERY:
	{
		return "MS_DISCOVERY";
	}break;
	case MS_BS:
	{
		return "MS_BS";
	}break;
	case MS_FAILURE:
	{
		return "MS_FAILURE";
	}break;

	default:
		return "MS_NONE";
	}
		}

float getLDCRate(unsigned short index)
{
	switch(index)
	{
	case 251:
		return 1.0f/500.0f;

	case 250:
		return 1.0f/250.0f;

	case 100:
		return 1.0f/100.0f;

	case 50:
		return 1.0f/50.0f;

	case 25:
		return 1.0f/25.0f;

	case 10:
		return 1.0f/10.0f;

	case 5:
		return 1.0f/5.0f;

	case 2:
		return 1.0f/2.0f;

	case 1:
		return 1.0f/1.0f;

	case 11:
		return 2.0f;
	case 12:
		return 5.0f;
	case 13:
		return 10.0f;
	case 14:
		return 30.0f;

	case 15:
		return 60.0f;
	case 16:
		return 120.0f;
	case 17:
		return 300.0f;
	case 18:
		return 600.0f;
	case 19:
		return 1800.0f;
	case 20:
		return 3600.0f;


	}
	return 0.2;
}

void send_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
		struct soap* msg, u_char* buf, ssize_t len) {

	printf("send_buf called\n");

	return;
}

ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
		struct soap* msg, char **buf) {

	printf("rcv_buf called\n");
	return 0;
}

void discovery_set_device(char * discdevice)
{
	if(discdevice!=NULL)
	{
		device = discdevice;
	}
}

void *discovery_worker_loop();

int discovery_worker_init() {
	device_proxy_init_device_list();
	service_cache_init();

	if(!openSerial())
	{
		printf("Unable to open connection to base station\n");
		exit(1);
	}

	printf("Discovering active nodes\n");
	// Ping to discover existing nodes
	unsigned short usNode = 0;

	for(usNode = 0;usNode < 1000;usNode++)
	{
		if(pingNode(usNode))
		{
			printf("Found node %d\n",usNode);
			registerNode(usNode);
		}

	}
	tcflush(handle,TCIOFLUSH);
	device_proxy_list_get_device(initLDCDevice,NULL);
	printf("Microstrain discovery initialized successfully\n");

#ifdef WIN32
	discovery_worker = CreateThread (NULL, 0, (DWORD WINAPI) discovery_worker_loop, NULL, 0, &workerid);
	if (discovery_worker != NULL)
#else
		if (pthread_create(&discovery_worker, NULL, discovery_worker_loop, NULL)
				== 0)
#endif
		{
			return 0;
		} else {
			return 1;
		}
}

int gateway_running() {
	return !shutdownFlag_discovery;
}

void discovery_worker_shutdown() {
	shutdownFlag_discovery = 1;
}

void discovery_worker_wait() {

#ifdef WIN32
	WaitForSingleObject(discovery_worker, INFINITE);
#else
	pthread_join(discovery_worker, NULL);
#endif
}

void *discovery_worker_loop() {

	while(gateway_running())
	{
		// Sleep
		sleep(1);

		// Read next packet
		readMessage(1);

		// Process packets
		while(inMessageQueue(MS_DISCOVERY))
		{
			msmessage msg;
			getMessageFromQueue(MS_DISCOVERY,&msg);
			processDiscovery(&msg);
		}

		while(inMessageQueue(MS_LDC))
		{
			msmessage msg;
			getMessageFromQueue(MS_LDC,&msg);
			processLDC(&msg);
		}

	}

	// Stop all nodes
	device_proxy_list_get_device(stopDevice,NULL);

	closeSerial();

	printf("\nDiscovery: Listened long enough\n");
	fflush(stdout);
	device_proxy_list_cleanup();
	printf("\nDiscovery:exiting\n");
	fflush(stdout);
	return NULL;
}

void discovery_notify_unregister(struct remote_device *rem_device)
{
	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_device);
	stopNode(msdev->id);
	printf("Removed device %d\n",msdev->id);
}

