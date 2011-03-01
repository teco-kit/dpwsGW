//#include "acs_inv1.nsmap"
// Set FD_SETSIZE to 1024 because Cygwin default is only 64 while Linux default is 1024
#define FD_SETSIZE 1024
#include <ws4d-gSOAP/dpws_device.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <write_bits_buf.h>
//#include <read_bits_buf.h>
#include <assert.h>

#include "../../discover.h"
#include "../../proxy_handler.h"
#include "../../proxy_structures.h"
#include "../../service_cache.h"

#include "SensorValues_operations.h"
//TODO use dynamic loading!!
#define get_event_func(FUNC_PTR,MODEL)\
		do{\
			extern void MODEL##_event;\
			*((void **)FUNC_PTR)=&MODEL##_event;\
		}while(0)

#ifndef WIN32
#include <pthread.h>
#endif

#define MAXBUFLEN 100

//#define DUMMY_NODE_CONTIKI  //comment this out to build against the "original" dummynode
/* handle for multithreading */
#ifdef WIN32
DWORD workerid;
HANDLE discovery_worker;
#else
pthread_t discovery_worker;
#endif

typedef union {
	struct {
		uint16_t svc;
		uint8_t op;
		uint8_t len;
	} msg;
	char buf[4];
} header;

typedef struct {
	int op_sock;
	int ev_sock;
} socket_connection;

#define remote_device_get_operation_socket(rem_dev) ((socket_connection*)remote_device_get_addr(rem_dev))->op_sock
#define remote_device_get_eventing_socket(rem_dev) ((socket_connection*)remote_device_get_addr(rem_dev))->ev_sock

static int has_ev_sock(void *c, void *ev_sock) {
	return ((socket_connection *) c)->ev_sock == *(int *) ev_sock;
}
#define device_proxy_list_get_device_by_ev_sock(ev_sock) device_proxy_list_get_device(has_ev_sock,&ev_sock)

int shutdownFlag_discovery = 0; /* Flag indicates systemshutdown. */

void discovery_set_device(char * discdevice)
{

}

void discovery_set_address(int begin, int end)
{

}

void *discovery_worker_loop();

int discovery_worker_init() {
	device_proxy_init_device_list();
	service_cache_init();

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

static int discovery_process_hello_msg(const char *msg, int *ev_sock,
		int *op_sock, struct sockaddr_storage *rem_address);

static int listener_init(char *port, struct addrinfo **host_results) {
	struct addrinfo hints;
	char *ipver = NULL;

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty

#ifdef DUMMY_NODE_CONTIKI
	hints.ai_family = AF_INET6; // IPv6
#else
	hints.ai_family = AF_INET; // IPv4
#endif

	hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
	hints.ai_flags = AI_PASSIVE;

	{
		int res;
		// get ready to connect
		if ((res = getaddrinfo(NULL, port, &hints, host_results)) != 0) {

			fprintf(
					stderr,
					"no idea what, but something went wrong setting up the address.%s",
					gai_strerror(res));
			return 1;
		}
	}

	ipver = ((*host_results)->ai_family == AF_INET) ? "IPv4" : "IPv6";

	printf(
			"setup of the listener, port %s did work, IP protocol version is %s\n",
			port, ipver);

	return 0;
}

// get sockaddr, IPv4 or IPv6:
void *discovery_get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

static int tcp_connect_persistent(char *remote_ip, char *remote_port) {
	struct addrinfo hints;
	struct addrinfo *remote_info;
	int sockfd;
	int rv;

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty

#ifdef DUMMY_NODE_CONTIKI
	hints.ai_family = AF_INET6; // IPv6
#else
	hints.ai_family = AF_INET; // IPv4
#endif

	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	if ((rv = getaddrinfo(remote_ip, remote_port, &hints, &remote_info)) != 0) {
		fprintf(stderr, "%s:%s\n", remote_ip, remote_port);
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	if ((sockfd = socket(remote_info->ai_family, remote_info->ai_socktype,
			remote_info->ai_protocol)) == -1) {
		perror("node: socket");
		goto end;
	}

	if (connect(sockfd, remote_info->ai_addr, remote_info->ai_addrlen) == -1) {
		close(sockfd);
		sockfd = -1;
		perror("client: connect");
		goto end;

	} else {
		goto end;
	}

	end: freeaddrinfo(remote_info);
	return sockfd; // -1 in error case, >= 0 on success

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

	int op_sock;

	size_t sent_len;

	printf("sent service id %u", service_id);

	header in = { { service_id, op_id, len } };

	struct remote_device *rem_dev = get_device(msg);
	last_rem_dev = rem_dev;

	if (rem_dev == NULL) {
		printf("No device matching %s found.", wsa_header_get_To(msg));
		return 0;
	}

	{

		op_sock = remote_device_get_operation_socket(rem_dev);
	}
	assert(op_sock > 0);

	//build the message from buf, put it on the socket
	if ((sent_len = send(op_sock, in.buf, sizeof(in), 0)) != sizeof(in)) {
		perror("send");
		return 0;
	}
	sent_len = 0;

	if (in.msg.len)
		if ((sent_len = send(op_sock, buf, len, 0)) != in.msg.len) {
			perror("send");
			return 0;
		}

	printf("send op[%d,%d] to %s with length %lu\n", in.msg.svc, in.msg.op,
			wsa_header_get_To(msg), (long int) sent_len);

	return 1;
}

ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
		struct soap* msg, char **buf) {

	ssize_t rcv_len = 0;
	header ret;
	*buf = NULL;
	int op_sock;

	//	struct remote_device *rem_dev = get_device(msg);
	//TODO: FIX generated API!!!
	struct remote_device *rem_dev = last_rem_dev;

	if (rem_dev == NULL) {
		printf("No device matching %s found.", wsa_header_get_To(msg));
		return -1;
	}

	op_sock = remote_device_get_operation_socket(rem_dev);

	if (recv(op_sock, &ret, sizeof(ret), 0) != sizeof(ret))
		return -1;
	if (!ret.msg.len)
		return 0;

	assert(ret.msg.op==op_id);

	*buf = malloc(ret.msg.len);

	if ((rcv_len = recv(op_sock, *buf, ret.msg.len, 0)) != ret.msg.len) {
		if (rcv_len == -1)
			perror("recv");
		printf("there was a problem in receiving data from node %s \n",
				wsa_header_get_To(msg));
		return -1;
	} else {
		printf("received reply with length %lu\n", (long int) rcv_len);
	}

	return rcv_len;
}

static int handle_node_message(int sockfd) {
	ssize_t buf_len = 0;
	header evt;
	char *buf = NULL;
	struct remote_device *rem_device = device_proxy_list_get_device_by_ev_sock(
			sockfd);

	struct dpws_s *device = rem_device ? remote_device_get_dpws_device(
			rem_device) : NULL;

	if (!device) {
		printf("device for event not found");
		return -1;
	}

	if (recv(sockfd, &evt, sizeof(evt), 0) != sizeof(evt))
		return -1;

	if (evt.msg.len)
		buf = malloc(evt.msg.len);

	if ((buf_len = recv(sockfd, buf, evt.msg.len, 0)) != evt.msg.len) {
		if (buf_len == -1)
			perror("recv");
		printf("there was a problem in receiving data from node\n");
		return -1;
	} else {
		printf("received event[%d,%d] with length %lu on %s\n", evt.msg.svc,
				evt.msg.op, (long int) buf_len, device->hosting_id);
	}

	{
		void (*deliver_event)(int, int, void *, char *, size_t);
		get_event_func(&deliver_event,SSimpDevice);

		deliver_event(evt.msg.svc, evt.msg.op, device, buf, evt.msg.len);
	}

	if (buf)
		free(buf);
	return 0;
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

static int discovery_get_new_node(struct sockaddr_storage *remote_address,
		char *port, char *op_port, int *ev_fd, int *op_fd) {
	char remote_ip[INET6_ADDRSTRLEN + 5];/* %tap0 for example: provide the device for linklocal adresses */
	int eventing_fd;
	int operation_fd;

	printf("discovery_worker: discovered node %s, will connect to port %s\n",
			(char *) inet_ntop(remote_address->ss_family,
					discovery_get_in_addr((struct sockaddr *) remote_address),
					remote_ip, sizeof(remote_ip)), port);

#ifdef DUMMY_NODE_CONTIKI
	/* add the device specifier */
	strcat(remote_ip, "%tap0");
#endif

	eventing_fd = tcp_connect_persistent(remote_ip, port);
	operation_fd = tcp_connect_persistent(remote_ip, op_port);

	if (eventing_fd > 0 && operation_fd > 0) {
		printf("Got a connection on port %s to a node which has the ip %s.\n",
				port, remote_ip);
		*ev_fd = eventing_fd;
		*op_fd = operation_fd;

		return 0;
	} else {
		printf("could not connect to node with ip %s, port %s\n", remote_ip,
				port);
		return -1;
	}
}

static int process_discovery(int discovery_sock) {
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len = sizeof(buf);

	int eventing_sock = 0;
	int operation_sock = 0;

	int numbytes = 0;

	printf("running process_discovery\n");
	if ((numbytes = recvfrom(discovery_sock, buf, MAXBUFLEN - 1, 0,
			(struct sockaddr *) &their_addr, &addr_len)) == -1) {
		return -1; // no new node discovered, receive failed
	} else {
		buf[numbytes] = '\0';
		printf("received message %s\n", buf);

		if (discovery_process_hello_msg(buf, &eventing_sock, &operation_sock,
				&their_addr) == 0) {
			return eventing_sock;
		}

		return -1; //something went wrong: message has wrong format or connection failed
	}
}

static int setup_discovery_sock(char* port) {
	struct addrinfo *servinfo;
	int discovery_sock;
	int opt = 1;

	if (listener_init(port, &servinfo) != 0) {
		fprintf(stderr, "problem with the adress setup occured, giving up");
		return (-1);
	}

	if ((discovery_sock = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1) {
		perror("socket");
		return discovery_sock; // (-1) We are not able to discover, end of all days
	}

	if (setsockopt(discovery_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
			sizeof(opt)) < 0) {
		perror("setsockopt REUSEADDR");
		exit(1);
	}

	if (bind(discovery_sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		perror("bind");
		close(discovery_sock);
		return -1; // We are not able to discover, end of all days
	}

	//we don't need it anymore...
	freeaddrinfo(servinfo);

	return discovery_sock;
}

static int discovery_get_node_info(const char *msg, char **port,
		char **op_port, char **version, char **model) {
	char *tok = NULL;
	char msg_copy[strlen(msg)];
	char delim[] = "_";

	strcpy(msg_copy, msg);

	if (msg != NULL) {
		tok = strtok(msg_copy, delim);
		if (tok == NULL || strcmp(tok, "Hello") != 0) {
			return 1; //message in wrong format
		}

		if ((tok = strtok(NULL, delim)) != NULL) {
			*port = strdup(tok);

		} else {
			return 1;
		}

		if ((tok = strtok(NULL, delim)) != NULL) {
			*op_port = strdup(tok);

		} else {
			return 1;
		}

		if ((tok = strtok(NULL, delim)) != NULL) {
			*model = strdup(tok);
		} else {
			return 1;
		}

		if ((tok = strtok(NULL, delim)) != NULL) {
			*version = strdup(tok);
		} else {
			return 1;
		}
		return 0; //SUCCESS
	}
	return 1;
}

static int discovery_build_node_identifier(char *port,
		struct sockaddr_storage *rem_address, char **serial_num,
		char ** friendly_name) {

	size_t serial_len = (INET6_ADDRSTRLEN + 6) * sizeof(char);
	size_t name_len = serial_len + sizeof("DummyNode ");
	*serial_num = malloc(serial_len); //How to get the MAC of the node? the strange magic number (6) is for the port (16 bit)
	*friendly_name = malloc(name_len);

	inet_ntop(rem_address->ss_family, discovery_get_in_addr(
			(struct sockaddr *) rem_address), *serial_num, serial_len);

	strcat(*serial_num, "-");
	strcat(*serial_num, port);

	strcpy(*friendly_name, "DummyNode ");
	strcat(*friendly_name, *serial_num);

	return 0;
}

/* process the information a node sends with the discovery message */
static int discovery_process_hello_msg(const char *msg, int *ev_sock,
		int *op_sock, struct sockaddr_storage *rem_address) {

	char *port = NULL, *op_port = NULL, *version = NULL, *model = NULL;
	char *serial_num = NULL, *friendly_name = NULL;
	int result =
			discovery_get_node_info(msg, &port, &op_port, &version, &model);

	if (result == 0) {
		result = discovery_get_new_node(rem_address, port, op_port, ev_sock,
				op_sock);
	}

	discovery_build_node_identifier(port, rem_address, &serial_num,
			&friendly_name);

	if (result == 0) {
		socket_connection addr = { *op_sock, *ev_sock };
		result = dpws_device_build_and_register(model, &addr, sizeof(addr),
				gateway_get_interface(), version, serial_num, friendly_name);
	}

	free(port);
	free(op_port);
	free(version);
	free(model);
	free(friendly_name);
	free(serial_num);

	return result;
}

void *discovery_worker_loop() {
	int discovery_sock;
	struct timeval timeout;
	int new_sock;
	/* We are only listening to the nodes, either for UDP discovery messages
	 * or messages over TCP connections
	 */
	fd_set master;
	fd_set read_fds;
	fd_set err_fds;
	int fd_max;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_ZERO(&err_fds);

	/* setup of discovery socket */
	if ((discovery_sock = setup_discovery_sock("3491")) == -1) {
		exit(1);
	}

	FD_SET(discovery_sock, &master);
	fd_max = discovery_sock;

	while (!shutdownFlag_discovery) {
		int i;

		/* build a timeout (neccessary for being able to shut down the gateway */
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000; //might be a good compromise between fast reaction and low load
		memcpy(&read_fds,&master,sizeof(fd_set));
		memcpy(&err_fds,&master,sizeof(fd_set));
		//FD_COPY(&master,&read_fds);
		//FD_COPY(&master,&err_fds);
		//read_fds = master;
		//err_fds = master;

		if (select(fd_max + 1, &read_fds, NULL, &err_fds, &timeout) == -1) {

			perror("select "__FILE__);
			//exit(4);
		}

		//TODO: we'll have to use the ip (or something similar) as identifier here, not a socket!
		for (i = 0; i <= fd_max; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == discovery_sock) {
					printf("\nsome node sent a discovery signal, will recvfrom on sock %d\n", i);
					if ((new_sock = process_discovery(i)) != -1) {
						if(new_sock < FD_SETSIZE)
						{
							if (new_sock > fd_max) {
								fd_max = new_sock;
							}
							FD_SET(new_sock, &master);
						} else {
							printf("fd_set is full, new socket will not be checked\n");
						}
					} else {
						printf("Adding new node failed\n");
					}
				} else {
					printf("\nreceived a message on socket %d\n", i);
					if (handle_node_message(i) == -1) {
						dpws_device_unregister(
								device_proxy_list_get_device_by_ev_sock(i));
						FD_CLR(i, &master);
						close(i);
						printf("discarded one node\n");
					}
				}
			}

			if (FD_ISSET(i, &err_fds)) {
				printf("\nWe got a problem here, socket %d\n", i);
				FD_CLR(i, &master);
			}
		}
		sched_yield();
	}
	close(discovery_sock);
	device_proxy_list_cleanup();
	printf("\nDiscovery: Listened long enough\n");
	fflush(stdout);
	return NULL;
}

void discovery_notify_unregister(struct remote_device *rem_device) {
	int persistent_sock = remote_device_get_eventing_socket(rem_device);
	close(persistent_sock);
}
