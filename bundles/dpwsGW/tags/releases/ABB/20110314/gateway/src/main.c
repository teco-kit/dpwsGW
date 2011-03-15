#include "discover.h"
#include "proxy_structures.h"
#include "proxy_handler.h"
#include "mutex_handling.h"
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


int main(int argc, char **argv)
{
#ifndef WIN32
	struct sigaction sa;
#endif
	char *interface = NULL;
	char *uuid = NULL;
	char * device = NULL;
	int begin = 0;
	int end = 1000;

	/* parsing command line options */
	while (argc > 1) {
		if (argv[1][0] == '-') {
			char *option = &argv[1][1];

			switch (option[0]) {
			case 'i': /* set interface with option -i */
				if (strlen(option) > 2) {
					++option;
					interface = option;
				} else {
					--argc;
					++argv;
					interface = argv[1];
				}
#ifdef DEBUG
				printf ("\nGateway: Set interface to \"%s\"\n", interface);
#endif
				break;
			case 'u': /* set id with option -u */
				if (strlen(option) > 2) {
					++option;
					uuid = option;
				} else {
					--argc;
					++argv;
					uuid = argv[1];
				}
#ifdef DEBUG
				printf ("\nGateway: Set uuid to \"%s\"\n", uuid);
#endif
				break;
			case 'd': /* set device with option -d */
							if (strlen(option) > 2) {
								++option;
								device = option;
							} else {
								--argc;
								++argv;
								device = argv[1];
							}
			#ifdef DEBUG
							printf ("\nGateway: Set device to \"%s\"\n", device);
			#endif
							break;
			case 'b': /* set begin with option -b */
										if (strlen(option) > 2) {
											++option;
											begin = atoi(option+2);
										} else {
											--argc;
											++argv;
											begin = atoi(argv[1]);
										}
						#ifdef DEBUG
										printf ("\nGateway: Set node address begin to \"%i\"\n", begin);
						#endif
										break;
			case 'e': /* set end with option -e */
													if (strlen(option) > 2) {
														++option;
														end = atoi(option+2);
													} else {
														--argc;
														++argv;
														end = atoi(argv[1]);
													}
									#ifdef DEBUG
													printf ("\nGateway: Set node address end to \"%i\"\n", end);
									#endif
													break;
			default:
				fprintf(stderr, "\nGateway: Bad option %s\n", argv[1]);
				printf("\n%s -i [interface address] -u urn:uuid[uuid] -d [discovery device] -b [node address begin] -e [node address end]\n",
					argv[0]);
				exit(1);
			}
		}
		--argc;
		++argv;
	}

	if (interface == NULL) {
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
				        	interface=calloc(32,sizeof(char));
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
		 exit(1);
		}
	}
	printf("\nGateway: set interface to %s\n",interface);
	gateway_set_interface(interface);

	/* initialize the mutex used for synchronising the two threads using the proxy datastructures */
	gateway_mutex_init();

	discovery_set_device(device);
	discovery_set_address(begin,end);

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

