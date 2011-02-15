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

#include "Acceleration_operations.h"
#include "DeviceInfo_operations.h"
#include "DataLogging_operations.h"

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

static pthread_mutex_t gw_mutex = PTHREAD_MUTEX_INITIALIZER;

int gw_state = GATEWAY_IDLE;

// Needed for messages without receiver
msdevice * gw_cur_device = NULL;

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
	pthread_mutex_lock(&gw_mutex);
	int ret = close(handle);
	if(ret==-1)
	{
		perror(NULL);
		return 0;
	}
	pthread_mutex_unlock(&gw_mutex);
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
		return -1;
	}

	memset(msgqueue[msgslot].buffer,0,sizeof(unsigned char)*266);

	// Read a byte and determine package type
	bytes_read = read(handle,msgqueue[msgslot].buffer,1);

	if(bytes_read==-1)
	{
		perror(NULL);
		return -1;
	}

	if(bytes_read!=1)
	{
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
			return -1;
		}
		if(bytes_read!=4)
		{
			printf("Failed to read rest of MS_READVALUE message\n");
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
			return -1;
		}

		if(bytes_read!=266)
		{
			printf("Failed to read rest of MS_DATAPAGE message\n");
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
					return -1;
				}

				if(bytes_read!=9)
				{
					printf("Failed to read rest of MS_LONGPING message\n");
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
					return -1;
				}

				if(bytes_read!=9)
				{
					printf("Failed to read rest of MS_LDC message, only received %d bytes\n",bytes_read);
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
					return -1;
				}

				if(bytes_read!=remainingsize)
				{
					printf("Failed to read rest of MS_LDC message\n");
					return -1;
				}
				msgqueue[msgslot].length += remainingsize;

				msgqueue[msgslot].type = MS_LDC;

			} else if(msgqueue[msgslot].buffer[0]==0x01 && msgqueue[msgslot].buffer[1]==0x05)
			{
				printf("High speed streaming is currently not supported\n");
				return -1;
				// TODO: Deal with High speed streaming packets, length may be greater than 266
			} else if(((msgqueue[msgslot].buffer[0] & 0x08) && msgqueue[msgslot].buffer[1]==0x07))
			{
				bytes_read = read(handle,msgqueue[msgslot].buffer,10);
				if(bytes_read==-1)
				{
					perror(NULL);
					return -1;
				}

				if(bytes_read!=10)
				{
					printf("Failed to read rest of MS_DISCOVERY message\n");
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
		return -1;

	}break;

	default:
	{
		printf("Unknown message type\n");
		return -1;
	}

	}

	//printf("%s message received\n",msmessagestr(msgqueue[msgslot].type));
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
	initLDC(id,5);
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
	dev.state = DEVICE_IDLE;

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
		getMessageFromQueue(MS_FAILURE,&versionmsg);
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
		checksum=(checksum+msg->buffer[i])%65535;
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
		delta = tick;
	} else {
		// Wrap around
		delta = USHRT_MAX + tick;
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
	ldcmessage ldcmsg;

	memcpy(ldcmsg.values,msdev->lastvalue,sizeof(float)*8);
	ldcmsg.timertick = tick;
	ldcmsg.delta = delta;
	AccelModel_event(SRV_Acceleration,OP_GetAccelerationValuesEvent,device,(char*)&ldcmsg,sizeof(ldcmessage));

}

int processLogging(msmessage * msg)
{
	int checksum = 0x05;
	int i =0;
	int next = 1;
	for(i=0;i<264;i++)
	{
		checksum = (checksum + msg->buffer[i]) % 65535;
	}
	if(checksum!=(msg->buffer[264]<<8)+msg->buffer[265])
	{
		printf("Error downloading data page: Invalid checksum\n");
		return 0;
	}
	int start = 0;
	int end = 264;
	// Look for session headers
	for(i=0;i<263;i++)
	{
		if(msg->buffer[i]==255&&msg->buffer[i+1]==255)
		{
			if(gw_cur_device->session.page==2 && start == 0)
			{
				start = i+11;
			} else {
				end = i;
				next = 0;
				break;
			}
		}
	}
	// Get channels
	loggingpage page;
	page.channelmask = gw_cur_device->session.channelmask;
	page.channels = 0;
	page.samplecount = 0;
	page.rate = gw_cur_device->session.rate;
	page.prevsamplecount = gw_cur_device->session.samplecount;
	memset(page.samples,0,sizeof(loggingsample)*132);
	for(i=0;i<8;i++)
	{
		if ((page.channelmask >> i) & 1)
		{
			page.channels++;
		}
	}
	if(page.channels==0)
	{
		printf("Invalid channelmask, no channels specified\n");
		return 0;
	}

	// Get first channel used
	int channel = 0;
	for(channel=0;channel<8;channel++)
	{
		if ((page.channelmask >> channel) & 1)
		{
			break;
		}
	}
	loggingsample cursample;
	memset(cursample.values,0,sizeof(float)*8);
	unsigned char msb;
	unsigned char lsb;
	int nexttype = 0;

	if(gw_cur_device->session.wrap!=0)
	{
		for(i=0;i<gw_cur_device->session.wrap;i++)
		{
			if(nexttype==0)
			{
				nexttype++;
				msb = gw_cur_device->session.wrappedval[i];
			} else {
				nexttype = 0;
				lsb = gw_cur_device->session.wrappedval[i];
				cursample.values[channel] = (msb << 8) + lsb;
				switch(gw_cur_device->channelaction[channel])
				{
				case 0x01:
				{
					// Strain
					cursample.values[channel] = gw_cur_device->gain[channel] * (cursample.values[channel] + gw_cur_device->offset[channel]);
				} break;

				case 0x02:
				{
					// G-Link
					if(gw_cur_device->gain[channel]!=0)
					{
						cursample.values[channel] =  (cursample.values[channel] - gw_cur_device->offset[channel]) / gw_cur_device->gain[channel];
						//printf("Read G-Link value: %f\n",msdev->lastvalue[i]);
					}
				} break;

				case 0x04:
				{
					// Custom formula
					cursample.values[channel] =  cursample.values[channel] * gw_cur_device->gain[channel] + gw_cur_device->offset[channel] ;
				} break;
				}
				// Get next channel
				int nextchannel = channel+1;
				for(;nextchannel<8+channel;nextchannel++)
				{
					if ((page.channelmask >> (nextchannel%8)) & 1)
					{
						break;
					}
				}
				if(nextchannel<=channel)
				{
					page.samples[page.samplecount] = cursample;
					page.samplecount++;
					gw_cur_device->session.samplecount++;
					memset(cursample.values,0,sizeof(float)*8);
				}
				channel = nextchannel;
			}
		}
	}

	for(i=start;i<end;i++)
	{
		if(nexttype==0)
		{
			nexttype++;
			msb = msg->buffer[i];

			// Keep track of wrapped values
			gw_cur_device->session.wrappedval[gw_cur_device->session.wrap]=msg->buffer[i];
			gw_cur_device->session.wrap++;
		} else {
			nexttype = 0;
			lsb = msg->buffer[i];
			// Keep track of wrapped values
			gw_cur_device->session.wrappedval[gw_cur_device->session.wrap]=msg->buffer[i];
			gw_cur_device->session.wrap++;
			// Set value using channel action
			cursample.values[channel] = (msb << 8) + lsb;
			switch(gw_cur_device->channelaction[channel])
			{
			case 0x01:
			{
				// Strain
				cursample.values[channel] = gw_cur_device->gain[channel] * (cursample.values[channel] + gw_cur_device->offset[channel]);
			} break;

			case 0x02:
			{
				// G-Link
				if(gw_cur_device->gain[channel]!=0)
				{
					cursample.values[channel] =  (cursample.values[channel] - gw_cur_device->offset[channel]) / gw_cur_device->gain[channel];
					//printf("Read G-Link value: %f\n",msdev->lastvalue[i]);
				}
			} break;

			case 0x04:
			{
				// Custom formula
				cursample.values[channel] =  cursample.values[channel] * gw_cur_device->gain[channel] + gw_cur_device->offset[channel] ;
			} break;
			}

			// Get next channel
			int nextchannel = channel+1;
			for(;nextchannel<8+channel;nextchannel++)
			{
				if ((page.channelmask >> (nextchannel%8)) & 1)
				{
					break;
				}
			}
			if(nextchannel<=channel)
			{
				page.samples[page.samplecount] = cursample;
				page.samplecount++;
				gw_cur_device->session.samplecount++;
				memset(cursample.values,0,sizeof(float)*8);
				gw_cur_device->session.wrap = 0;
				memset(gw_cur_device->session.wrappedval,0,sizeof(unsigned char)*16);
			}
			channel = nextchannel;
		}
	}
	gw_cur_device->session.lastmsg = msg;
	//Deliver events
	AccelModel_event(SRV_DataLogging,OP_DataLogging_DataEvent,device,(char*)&page,sizeof(loggingpage));

	return next;
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

void eraseSessions(unsigned short node)
{
	unsigned char	cmd[7] =	{	0x06,	//	erase node command byte
			node >> 8,	//	MSB of the node address
			node & 0xFF,	//	LSB of the node address
			0x08,
			0x10,
			0x0C,
			0xFF
	};

	int bytes = write(handle,cmd,7);
	if(bytes==-1)
	{
		printf("Error erasing node %d\n",node);
		perror(NULL);
	} else if(bytes!=7)
	{
		printf("Unable to erase node %d\n",node);
	}

	msmessage msg;
	readMessage(0);
	while(!inMessageQueue(MS_ERASE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_ERASE))
	{
		getMessageFromQueue(MS_ERASE,&msg);
		printf("Erased node %d\n",node);
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		printf("Failed to erase node %d\n",node);
	}
}

void initLDC(unsigned short node,unsigned short rateindex)
{
	struct remote_device *rem_device = device_proxy_list_get_device_by_id(&node);
	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_device);

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

	// Set LDC rate
	eepromcmd[3] = 72;
	eepromcmd[4] = rateindex >>8;
	eepromcmd[5] = rateindex & 0xFF;

	/*	calculate the checksum	*/
	checksum = (eepromcmd[1] + eepromcmd[2] + eepromcmd[3] + eepromcmd[4] + eepromcmd[5]) & 0xFFFF;
	eepromcmd[6] = checksum >> 8;		//	set the MSB of the checksum
	eepromcmd[7] = checksum & 0xFF;	//	set the LSB of the checksum

	bytes = write(handle,eepromcmd,8);

	if(bytes==-1)
	{
		printf("Error writing node EEPROM\n");
		perror(NULL);
	} else if(bytes!=8)
	{
		printf("Unable to write node EEPROM, wrote only %d\n",bytes);
	}

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
	msdev->state = DEVICE_LDC;
	gw_state = GATEWAY_LDC;
}

void initLogging(unsigned short node,unsigned short rateindex)
{
	struct remote_device *rem_device = device_proxy_list_get_device_by_id(&node);
	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_device);

	// Set streaming duration to infinite
	unsigned char	eepromcmd[8] =	{	0x04,			//	eeprom write command byte
			node >> 8,	//	MSB of the node address
			node & 0xFF,	//	LSB of the node address
			102,		//	location to write
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

	// Set LDC rate
	eepromcmd[3] = 14;
	eepromcmd[4] = rateindex >>8;
	eepromcmd[5] = rateindex & 0xFF;

	/*	calculate the checksum	*/
	checksum = (eepromcmd[1] + eepromcmd[2] + eepromcmd[3] + eepromcmd[4] + eepromcmd[5]) & 0xFFFF;
	eepromcmd[6] = checksum >> 8;		//	set the MSB of the checksum
	eepromcmd[7] = checksum & 0xFF;	//	set the LSB of the checksum

	bytes = write(handle,eepromcmd,8);

	if(bytes==-1)
	{
		printf("Error writing node EEPROM\n");
		perror(NULL);
	} else if(bytes!=8)
	{
		printf("Unable to write node EEPROM, wrote only %d\n",bytes);
	}

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

	printf("Starting Logging\n");
	// Start LDC
	unsigned char loggingcmd[3] = {
			node >> 8,	//	MSB of Node Address
			node & 0xFF,	//	LSB of Node Address
			1
	};
	bytes = write(handle,loggingcmd,3);
	if(bytes==-1)
	{
		printf("Error starting Logging\n");
		perror(NULL);
	} else if(bytes!=10)
	{
		printf("Unable to start Logging\n");
	}

	readMessage(0);
	while(!inMessageQueue(MS_TRIGGERLOGGING))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_TRIGGERLOGGING))
	{
		getMessageFromQueue(MS_TRIGGERLOGGING,&msg);
		printf("Started node %d in Logging\n",node);
	}
	msdev->state = DEVICE_LOGGING;
}

void initDownload(unsigned short node)
{
	struct remote_device *rem_device = device_proxy_list_get_device_by_id(&node);
	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_device);

	// Get first page
	requestPage(node,2);

	msmessage msg;

	readMessage(0);
	while(!inMessageQueue(MS_DATAPAGE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}

	if(inMessageQueue(MS_DATAPAGE))
	{
		getMessageFromQueue(MS_DATAPAGE,&msg);
	} else if(inMessageQueue(MS_FAILURE))
	{
		getMessageFromQueue(MS_FAILURE,&msg);
		printf("Failed to get data page\n");
		return;
	}
	// Check checksum
	int checksum = 0x05;
	int i =0;
	for(i=0;i<264;i++)
	{
		checksum = (checksum + msg.buffer[i]) % 65535;
	}
	if(checksum!=(msg.buffer[264]<<8)+msg.buffer[265])
	{
		printf("Error downloading data page: Invalid checksum\n");
		return;
	}
	// Search for session header (does not deal with wrapped header because only first session is returned)
	msdev->session.page = 0;

	for(i=0;i<(264-12);i=i+2)
	{
		if(msg.buffer[i]==255 && msg.buffer[i+1]==255)
		{
			// Save session data
			msdev->session.page = 2;
			msdev->session.index = (msg.buffer[i+6] <<8) + msg.buffer[i+7];
			msdev->session.channelmask = (msg.buffer[i+8] <<8) + msg.buffer[i+9];
			msdev->session.lastmsg = &msg;
			msdev->session.wrap = 0;
			msdev->session.rate = getLoggingRate((msg.buffer[i+10] <<8) + msg.buffer[i+11]);
			memset(msdev->session.wrappedval,0,sizeof(char)*16);
			gw_state = GATEWAY_RECEIVING;
			msdev->state = DEVICE_SENDING;
			gw_cur_device = msdev;
			break;
		}
	}

}

int getSavedSessions(unsigned short node)
{
	int sessions = 0;

	requestEEPROM(node,4);
	readMessage(0);

	while(!inMessageQueue(MS_READVALUE)&&!inMessageQueue(MS_FAILURE))
	{
		readMessage(0);
	}
	msmessage msg;
	if(inMessageQueue(MS_READVALUE))
	{
		getMessageFromQueue(MS_READVALUE,&msg);
		sessions = (msg.buffer[0] << 8) + msg.buffer[1];

	} else if(inMessageQueue(MS_FAILURE)){
		getMessageFromQueue(MS_FAILURE,&msg);
		printf("Failed to get saved sessions from EEPROM\n");
	}

	return sessions;
}

void requestPage(unsigned short node, unsigned short page)
{
	unsigned char	cmd[5] =	{	0x05,	//	page download command byte
			node >> 8,	//	MSB of the node address
			node & 0xFF,	//	LSB of the node address
			page >> 8,	//	MSB of the location to read
			page & 0xFF	//	LSB of the location to read
	};

	int bytes = write(handle,cmd,5);
	if(bytes==-1)
	{
		printf("Error requesting page %d\n",page);
		perror(NULL);
	} else if(bytes!=5)
	{
		printf("Unable to request page %d\n",page);
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

	struct remote_device *rem_device = device_proxy_list_get_device_by_id(&node);
	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_device);
	msdev->state = DEVICE_IDLE;
	gw_state = GATEWAY_IDLE;
	return 1;
}

int stopDevice(void *in,void *dummy)
{
	msdevice * msdev=(msdevice*) in;
	pthread_mutex_lock(&gw_mutex);
	stopNode(msdev->id);
	pthread_mutex_unlock(&gw_mutex);
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

unsigned short getLDCRateIndex(char * rate)
{
	if(strcmp(rate,"500")==0)
	{
		return 251;
	} else if(strcmp(rate,"250")==0)
	{
		return 250;
	} else if(strcmp(rate,"100")==0)
	{
		return 100;
	} else if(strcmp(rate,"50")==0)
	{
		return 50;
	} else if(strcmp(rate,"25")==0)
	{
		return 25;
	} else if(strcmp(rate,"10")==0)
	{
		return 10;
	} else if(strcmp(rate,"5")==0)
	{
		return 5;
	} else if(strcmp(rate,"2")==0)
	{
		return 2;
	} else if(strcmp(rate,"1")==0)
	{
		return 1;
	} else if(strcmp(rate,"1/2")==0)
	{
		return 11;
	} else if(strcmp(rate,"1/5")==0)
	{
		return 12;
	} else if(strcmp(rate,"1/10")==0)
	{
		return 13;
	} else if(strcmp(rate,"1/30")==0)
	{
		return 14;
	} else if(strcmp(rate,"1/60")==0)
	{
		return 15;
	} else if(strcmp(rate,"1/120")==0)
	{
		return 16;
	} else if(strcmp(rate,"1/300")==0)
	{
		return 17;
	} else if(strcmp(rate,"1/600")==0)
	{
		return 18;
	} else if(strcmp(rate,"1/1800")==0)
	{
		return 19;
	} else if(strcmp(rate,"1/3600")==0)
	{
		return 20;
	}
	return 5;
}

unsigned short getLoggingRateIndex(char * rate)
{
	if(strcmp(rate,"2048")==0)
	{
		return 1;
	} else if(strcmp(rate,"1024")==0)
	{
		return 2;
	} else if(strcmp(rate,"512")==0)
	{
		return 3;
	} else if(strcmp(rate,"256")==0)
	{
		return 4;
	} else if(strcmp(rate,"128")==0)
	{
		return 5;
	} else if(strcmp(rate,"64")==0)
	{
		return 6;
	} else if(strcmp(rate,"32")==0)
	{
		return 7;
	}
	return 7;
}

float getLoggingRate(unsigned short index)
{
	switch(index)
	{
	case 1:
		return 1.0f/2048.0f;
	case 2:
		return 1.0f/1024.0f;
	case 3:
		return 1.0f/512.0f;
	case 4:
		return 1.0f/256.0f;
	case 5:
		return 1.0f/128.0f;
	case 6:
		return 1.0f/64.0f;
	case 7:
		return 1.0f/32.0f;
	}
	return 1.0f/32.0f;
}

const char * getDeviceInfoString(int state)
																		{
	switch(state)
	{
	case DEVICE_IDLE:
		return "Idle";
	case DEVICE_LDC:
		return "LDC";
	case DEVICE_LOGGING:
		return "Logging";
	case DEVICE_SENDING:
		return "Sending";
	}
	return "Idle";
																		}

static struct remote_device * get_device(struct soap* msg) {
	char *wsa_header = NULL;
	char *last_slash = NULL;
	char dev_uid[46] = { 0 };

	if (msg != NULL && ((wsa_header = wsa_header_get_To(msg)) != NULL)) {
		last_slash = strrchr(wsa_header, (int) '/');
	}

	if (last_slash != NULL) {
		strncpy(dev_uid, last_slash + 1, 45);
	}

	return device_proxy_list_get_device_by_dpws_device(dev_uid);

}

struct remote_device *last_rem_dev;

int send_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
		struct soap* msg, u_char* buf, ssize_t len) {



	struct remote_device *rem_dev = get_device(msg);
	last_rem_dev = rem_dev;

	if (rem_dev == NULL) {
		printf("No device matching %s found.", wsa_header_get_To(msg));
		return 1;
	}

	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_dev);

	switch(service_id)
	{
	case SRV_Acceleration:
	{
		switch(op_id)
		{
		case OP_StartLDC:
		{

			if(msdev->state!=DEVICE_IDLE)
			{
				return ACLERR_NotReady;
			}
			if(!(gw_state==GATEWAY_IDLE||gw_state==GATEWAY_LDC))
			{
				return ACLERR_GW_Busy;
			}

			LDCInfo * ldcinfo = (LDCInfo *) buf;
			pthread_mutex_lock(&gw_mutex);
			initLDC(msdev->id,getLDCRateIndex(ldcinfo->rate));
			pthread_mutex_unlock(&gw_mutex);
		}break;

		}
	}break;

	case SRV_DeviceInfo:
	{
		switch(op_id)
		{
		case OP_DeviceInfo_GetDeviceInfo:
		{
			return 1;
		}break;
		case OP_DeviceInfo_StopDevice:
		{
			pthread_mutex_lock(&gw_mutex);
			stopNode(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
		}break;
		}
	}break;

	case SRV_DataLogging:
	{
		switch(op_id)
		{
		case OP_DataLogging_StartLogging:
		{

			if(msdev->state!=DEVICE_IDLE)
			{
				return DLERR_NotReady;
			}
			if(!(gw_state==GATEWAY_IDLE||gw_state==GATEWAY_RECEIVING))
			{
				return DLERR_GW_Busy;
			}
			pthread_mutex_lock(&gw_mutex);
			int sessions = getSavedSessions(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
			if(sessions!=0)
			{
				return DLERR_NotCleared;
			}
			LoggingInfo * loginfo = (LoggingInfo*) buf;
			pthread_mutex_lock(&gw_mutex);
			initLogging(msdev->id,getLoggingRateIndex(loginfo->rate));
			pthread_mutex_unlock(&gw_mutex);
		}break;
		case OP_DataLogging_StartDownload:
		{

			if(msdev->state!=DEVICE_IDLE)
			{
				return DLERR_NotReady;
			}
			if(gw_state!=GATEWAY_IDLE)
			{
				return DLERR_GW_Busy;
			}
			pthread_mutex_lock(&gw_mutex);
			int sessions = getSavedSessions(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
			if(sessions<1)
			{
				return DLERR_NoSessions;
			}
			pthread_mutex_lock(&gw_mutex);
			initDownload(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
		}break;
		case OP_DataLogging_Erase:
		{
			if(msdev->state!=DEVICE_IDLE)
			{
				return DLERR_NotReady;
			}
			if(gw_state!=GATEWAY_IDLE)
			{
				return DLERR_GW_Busy;
			}

			pthread_mutex_lock(&gw_mutex);
			eraseSessions(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
		}break;
		case OP_DataLogging_GetSessionCount:
		{
			if(msdev->state!=DEVICE_IDLE)
			{
				return DLERR_NotReady;
			}
			if(gw_state!=GATEWAY_IDLE)
			{
				return DLERR_GW_Busy;
			}
		}break;
		}
	}break;

	}

	return 1;
}

ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
		struct soap* msg, char **buf) {

	struct remote_device *rem_dev = last_rem_dev;

	if (rem_dev == NULL) {
		printf("No device matching %s found.", wsa_header_get_To(msg));
		return -1;
	}

	msdevice *msdev=(msdevice *)remote_device_get_addr(rem_dev);

	switch(service_id)
	{
	case SRV_Acceleration:
	{
		switch(op_id)
		{
		case OP_StartLDC:
		{
			return 0;
		}break;
		}
	}break;

	case SRV_DeviceInfo:
	{
		switch(op_id)
		{
		case OP_DeviceInfo_GetDeviceInfo:
		{
			DeviceInfo * info = (DeviceInfo*)(*buf);
			strcpy(info->status,getDeviceInfoString(msdev->state));
			return strlen(info->status);
		}break;
		case OP_DeviceInfo_StopDevice:
		{
			return 0;
		}break;
		}
	}break;

	case SRV_DataLogging:
	{
		switch(op_id)
		{
		case OP_DataLogging_StartLogging:
		{
			return 0;

		}break;
		case OP_DataLogging_StartDownload:
		{
			return 0;

		}break;
		case OP_DataLogging_Erase:
		{
			return 0;
		}break;
		case OP_DataLogging_GetSessionCount:
		{
			sessioninfo * sessioncount = (sessioninfo*)(*buf);
			pthread_mutex_lock(&gw_mutex);
			sessioncount->count = getSavedSessions(msdev->id);
			pthread_mutex_unlock(&gw_mutex);
			return sizeof(sessioninfo);
		}break;
		}
	}break;

	}

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
	pthread_mutex_lock(&gw_mutex);
	for(usNode = 0;usNode < 1000;usNode++)
	{
		if(pingNode(usNode))
		{
			printf("Found node %d\n",usNode);
			registerNode(usNode);
		}

	}
	tcflush(handle,TCIOFLUSH);
	pthread_mutex_unlock(&gw_mutex);
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
		pthread_mutex_lock(&gw_mutex);
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

		while(inMessageQueue(MS_DATAPAGE))
		{
			msmessage msg;
			getMessageFromQueue(MS_DATAPAGE,&msg);
			if(processLogging(&msg))
			{
				gw_cur_device->session.page++;
				requestPage(gw_cur_device->id,gw_cur_device->session.page);
			} else {
				gw_cur_device = NULL;
				gw_state = GATEWAY_IDLE;
			}
		}

		// Process first logging page
		if(gw_cur_device!=NULL&&gw_cur_device->session.page==2)
		{
			if(processLogging(gw_cur_device->session.lastmsg))
			{
				gw_cur_device->session.page++;
				if(gw_cur_device->session.page<8192)
				{
					requestPage(gw_cur_device->id,gw_cur_device->session.page);
				}
			} else {
				gw_cur_device = NULL;
				gw_state = GATEWAY_IDLE;
			}
		}

		pthread_mutex_unlock(&gw_mutex);

	}

	// Stop all nodes
	device_proxy_list_get_device(stopDevice,NULL);
	pthread_mutex_lock(&gw_mutex);
	closeSerial();
	pthread_mutex_unlock(&gw_mutex);

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
	pthread_mutex_lock(&gw_mutex);
	stopNode(msdev->id);
	pthread_mutex_unlock(&gw_mutex);
	printf("Removed device %d\n",msdev->id);
}

