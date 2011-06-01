#include "discover.h"
#include "proxy_structures.h"
#include "proxy_handler.h"
#include "mutex_handling.h"

#include "minIni/minIni.h"
#include "options.h"

#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

void service_exit()
{
#ifdef DEBUG
	printf ("\nGateway: shutting down...\n");
#endif

	discovery_worker_shutdown();
}

int processGeneralSection(char * path)
{
	char interface[256];
	char uuid[256];
	memset(interface,0,sizeof(char)*256);
	memset(uuid,0,sizeof(char)*256);

	ini_gets("General","interface","",interface,256,path);
	if(strlen(interface)>0)
	{
		printf("Gateway: Read interface %s from config file\n",interface);
	} else {
		printf("Gateway: Found no interface in config file, using autodetection\n");
		struct ifaddrs * ifap;

		if (getifaddrs(&ifap) == 0)
		{
			struct ifaddrs * p = ifap;
			while(p)
			{
				struct sockaddr *a=p->ifa_addr;
				uint32_t addr  =((a)&&(a->sa_family == AF_INET)) ? ntohl(((struct sockaddr_in *)a)->sin_addr.s_addr) : 0;
				if (addr > 0 && p->ifa_flags & IFF_RUNNING && !(p->ifa_flags & IFF_LOOPBACK))
				{
					sprintf(interface, "%i.%i.%i.%i", (uint16_t)((addr>>24)&0xFF),(uint16_t)((addr>>16)&0xFF), (uint16_t)((addr>>8)&0xFF),(uint16_t)((addr>>0)&0xFF));
					break;
				}
				p = p->ifa_next;
			}
			freeifaddrs(ifap);
		}
		if(interface==NULL)
		{
			fprintf(stderr, "\nGateway: No interface address was specified!\n");
			return 0;
		}
	}
	printf("\nGateway: set interface to %s\n",interface);
	gateway_set_interface(interface);

	ini_gets("General","UUID","",uuid,256,path);

	return 1;
}

int main(int argc, char **argv)
{
#ifndef WIN32
	struct sigaction sa;
#endif

	char * path = "default.ini";

	/* parsing command line options */
	while (argc > 1) {
		if (argv[1][0] == '-') {
			char *option = &argv[1][1];

			switch (option[0]) {
			case 'c': /* set interface with option -i */
				if (strlen(option) > 2) {
					++option;
					path = option;
				} else {
					--argc;
					++argv;
					path = argv[1];
				}
#ifdef DEBUG
				printf ("\nGateway: Using config file \"%s\"\n", path);
#endif
				break;
			default:
				fprintf(stderr, "\nGateway: Bad option %s\n", argv[1]);
				printf("\n%s -c [config file]\n",
						argv[0]);
				exit(1);
			}
		}
		--argc;
		++argv;
	}

	if(!processGeneralSection(path)||!processPlatformSection(path))
	{
		fprintf(stderr, "\nGateway: Error reading ini-file\n");
		exit(1);
	}


	/* initialize the mutex used for synchronising the two threads using the proxy datastructures */
	gateway_mutex_init();

	/* start thread for udp discovery (is there a hello?) */
	if (!discovery_worker_init()) {
		printf("\nGateway: Starting node discovery...\n");
	}

	/* install signal handler for SIGINT or Ctrl-C */
#ifdef WIN32
	signal (SIGINT, service_exit);
#else
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = service_exit;
	sigaction(SIGINT, &sa, NULL);
#endif

	while (gateway_running()) {

		dpws_device_check_handles();

		device_proxy_list_check_subscriptions();
		sched_yield();
		//		pthread_yield();
	}
	printf("leaving main loop\n");
	fflush(stdout);
	discovery_worker_wait();
	return 0;
}

