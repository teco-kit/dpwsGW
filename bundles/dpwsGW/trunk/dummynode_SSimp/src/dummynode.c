#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <math.h>
#include "operations.h"


#define LISTENING_TIME 3 //defines the time in seconds the node listens between discovery calls
#define EVENTING_OFF	0
#define EVENTING_ON		1
#define OPERATION_OFF 	EVENTING_OFF
#define OPERATION_ON 	EVENTING_ON

#define MAX(x, y) x>y?x:y

static char *listening_port = "0";



static int mute=0;
static int simerror=0;
static int eventing_state = EVENTING_OFF;
static int operation_state = OPERATION_OFF;
static int sockfd, eventing_listener,operation_listener;


static int get_host_info_tcp_listener(char *port,
		struct addrinfo **host_results) {
	struct addrinfo hints;
	char *ipver = NULL;

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_INET; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;

	// get ready to connect
	if (getaddrinfo(NULL, port, &hints, host_results) != 0) {
		fprintf(stderr,
				"no idea what, but something went wrong setting up the address.");
		return 1;
	}

	ipver = ((*host_results)->ai_family == AF_INET) ? "IPv4" : "IPv6";

	/* the following output is not really true, only the first address
	 * found is here shown
	 */
	printf(
			"setup of the listener on port %s did work, IP protocol version is %s\n",
			port, ipver);

	return 0;
}

//caller is responsible for freeing addrinfo struct
static int get_host_info_udp_discovery(char *host, char *port,
		struct addrinfo **host_results) {
	struct addrinfo hints;
	char *ipver = NULL;

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets

	// get ready to connect
	if (getaddrinfo(host, port, &hints, host_results) != 0) {
		fprintf(stderr,
				"no idea what, but something went wrong setting up the address.");
		return 1;
	}

	ipver = ((*host_results)->ai_family == AF_INET) ? "IPv4" : "IPv6";

	/* the following output is not really true, only the first address
	 * found is here shown
	 */
	printf(
			"setup of the address %s, port %s did work, IP protocol version is %s\n",
			host, port, ipver);

	return 0;
}



static int send_hello_msg(int hello_sock, struct addrinfo *hello_info, int esocket, int osocket) {
	int numbytes;
	static char hello_msg[32];

	{
		struct sockaddr sa;
		socklen_t len = sizeof(sa);
		int eport,oport;
		getsockname(esocket, &sa, &len);
		eport=ntohs(((struct sockaddr_in*)&sa)->sin_port);
		getsockname(osocket, &sa, &len);
		oport=ntohs(((struct sockaddr_in*)&sa)->sin_port);

		sprintf(hello_msg, "Hello_%d_%d_1_0.001",eport,oport);

	}


	if ((numbytes = sendto(hello_sock, hello_msg, strlen(hello_msg), 0,
			hello_info->ai_addr, hello_info->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	} else {
		printf("sent hello message.\n");
	}
	return 0;
}

//return 0 when no discovery acknowledge message occured
int node_discovery_listen(int hello_sock, struct addrinfo *hello_info) {
	fd_set master;
	fd_set read_fds; // temp file descriptor list for select()

	struct addrinfo *servinfo = calloc(1, sizeof(struct addrinfo)); // will point to the results
	struct addrinfo *lservinfo = calloc(1, sizeof(struct addrinfo));
	struct sockaddr *remoteaddr = NULL;


	int port_num = atoi(listening_port);
	char op_port[6];

	int new_ev_fd;
	int new_op_fd = -1;
	int fd_max;
	socklen_t addrlen;
	struct timeval tv = { };

	sprintf(op_port, "%d", port_num?(port_num + 1):0);

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	if (get_host_info_tcp_listener(listening_port, &servinfo) != 0) {
		fprintf(stderr, "problem with setting up the listener, giving up");
		exit(1);
	} else {
		eventing_listener = socket(servinfo->ai_family, servinfo->ai_socktype,
				lservinfo->ai_protocol);

		if (eventing_listener == -1) {
			fprintf(stderr, "sockets could not be connected");
			exit(1);
		}

		if (bind(eventing_listener, servinfo->ai_addr, servinfo->ai_addrlen)
				< 0) {
			close(eventing_listener);
			perror("bind");
			exit(1);
		}
		FD_SET(eventing_listener, &master);
	}
	freeaddrinfo(servinfo);


	if (get_host_info_tcp_listener(op_port, &servinfo) != 0) {
		fprintf(stderr,
				"problem with setting up the operation listener, giving up");
		exit(1);
	} else {
		operation_listener = socket(servinfo->ai_family, servinfo->ai_socktype,
				servinfo->ai_protocol);

		if (operation_listener == -1) {
			fprintf(stderr, "sockets could not be connected");
			exit(1);
		}

		if (bind(operation_listener, servinfo->ai_addr, servinfo->ai_addrlen)
				< 0) {
			close(eventing_listener);
			close(operation_listener);
			perror("bind");
			exit(1);
		}

		FD_SET(operation_listener, &master);
	}

	// not needed anymore
	freeaddrinfo(servinfo);

	// listen
	if (listen(eventing_listener, 1) == -1 || listen(operation_listener, 1)
			== -1) {
		perror("listen");
		close(eventing_listener);
		close(operation_listener);
		exit(3);
	}

	addrlen = sizeof(remoteaddr);

	fd_max = MAX(eventing_listener, operation_listener);

    send_hello_msg(hello_sock, hello_info, eventing_listener, operation_listener);


	//freeaddrinfo(lservinfo);
	while (1 /*use some variable to break on signal*/) {
		read_fds = master;
		tv.tv_sec = 0;
		tv.tv_usec = 13000;

		if (select(fd_max + 1, &read_fds, NULL, NULL, &tv) == -1) {
			perror("select");
		//	exit(4);
		}

		if (eventing_listener != -1 && FD_ISSET(eventing_listener, &read_fds)) {
			new_ev_fd = accept(eventing_listener,
					(struct sockaddr *) &remoteaddr, &addrlen);

			if (new_ev_fd == -1) {
				perror("accept");
				close(operation_listener);
				close(eventing_listener);
				return 2;
			} else {
				printf("A connection for eventing was accepted.\n");
				eventing_state = EVENTING_ON;
				FD_CLR(eventing_listener, &master);
				close(eventing_listener); //we have our connection, further ones are undesired
				eventing_listener = -1;
				FD_SET(new_ev_fd, &master);
				fd_max = MAX(new_ev_fd, fd_max);
			}


		} else if (operation_listener != -1 && FD_ISSET(operation_listener, &read_fds)) {
			new_op_fd = accept(operation_listener,
					(struct sockaddr *) &remoteaddr, &addrlen);

			if (new_op_fd == -1) {
				perror("accept");
				close(operation_listener);
				close(eventing_listener);
				return 2;
			} else {
				printf("A connection for operations was accepted.\n");
				operation_state = OPERATION_ON;
				FD_CLR(operation_listener, &master);
				close(operation_listener); //we have our connection, further ones are undesired
				operation_listener = -1;
				FD_SET(new_op_fd, &master);
				fd_max = MAX(new_op_fd, fd_max);
			}

		} else if (new_op_fd != -1 && FD_ISSET(new_op_fd, &read_fds)) {
			u_char buf[50];
			//TODO: build "real" operation r
			if ( simerror)
			{
				printf("Dummynode: ignoring operation\n");
			}
			else if (!handle_request(new_op_fd)){
				printf("Dummynode: we got some operation\n");
			} else {
				printf("Dummynode: operation failed\n");
				return 1;
			}
		} else {
			if (eventing_state == EVENTING_OFF) {
				printf("Dummynode: Timed out without discovery acknowledgement.\n");
			    //we are not yet discovered, send further hello messages
			} else {
				if(mute)
				{
					  printf("%c\b", 'O');
				}	
				else if ( send_event(new_ev_fd,0,OP_SensorValues_SensorValuesEvent)) {
					printf("Dummynode: sending event msg failed, connection to gateway lost\n");
					return 1;
				}
				else
				{
					  static int pos=0;
					  char cursor[4]={'/','-','\\','|'};
					  printf("%c\b", cursor[pos%sizeof(cursor)]);
					  fflush(stdout);
					  pos++;
				}
			}
		}
	}
	return 0;
}


void service_exit() {
	printf("\nDummynode: shutting down...\n");
	close(sockfd);
	close(eventing_listener);
	close(operation_listener);
	/* We'll have to leave the eventing loop */
	if (eventing_state == EVENTING_ON) {
		eventing_state = EVENTING_OFF;
	}
	exit(0);
}

void _mute()
{
	fputs("\nmute ",stderr);
	mute=!mute;
	fputs(mute?"on":"off",stderr);
}

void _simerror()
{
	fputs("\nborked ",stderr);
	simerror=!simerror;
	fputs(simerror?"on":"off",stderr);
}


int main(int argc, char *argv[]) {
    char *bcast_addr="127.0.0.1";
    char _addr[32];

	struct addrinfo *servinfo = calloc(1, sizeof(struct addrinfo)); // will point to the results
	int broadcast = 1;
	init_sensors();


	signal(SIGINT, service_exit);
        signal(SIGHUP,_mute);
	siginterrupt(SIGHUP,0);
        signal(SIGUSR1,_simerror);
	siginterrupt(SIGUSR1,0);


	if (argc == 2) {

		listening_port = argv[1];
	}

	if (argc == 3) {
		bcast_addr= argv[1];
		listening_port = argv[2];
	}
	else
	{
		{
			struct ifaddrs * ifap;

			if (getifaddrs(&ifap) == 0)
			{
				struct ifaddrs * p = ifap;
				while(p)
				{
					struct sockaddr *a=p->ifa_broadaddr;
					uint32_t addr  =((a)&&(a->sa_family == AF_INET)) ? ntohl(((struct sockaddr_in *)a)->sin_addr.s_addr) : 0;
					if (addr > 0 && p->ifa_flags & IFF_RUNNING)
					{
						bcast_addr=_addr;
						sprintf(bcast_addr, "%i.%i.%i.%i", (uint16_t)((addr>>24)&0xFF),(uint16_t)((addr>>16)&0xFF), (uint16_t)((addr>>8)&0xFF),(uint16_t)((addr>>0)&0xFF));
						break;
					}
					p = p->ifa_next;
				}
				freeifaddrs(ifap);
			}
		}
	}

	//port_num = 9;



	if (get_host_info_udp_discovery(bcast_addr, "3491", &servinfo) != 0) {
		fprintf(stderr, "problem with setting up the adress, giving up.");
		exit(1);
	}

	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol);
	if (sockfd == -1) {
		fprintf(stderr, "socket could not be connected");
		exit(1);
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
			sizeof broadcast) == -1) {
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}

	//TODO: listener comes here, timeout after LISTENING_TIME seconds.
	if (node_discovery_listen(sockfd, servinfo) != 1) {
		printf("failure when trying to get a connection to the gateway");
	}

	freeaddrinfo(servinfo); // free the linked-list
	close(sockfd);

	return 0;
}
