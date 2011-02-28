	//#include "acs_inv1.nsmap"
	#include <ws4d-gSOAP/dpws_device.h>
	#include <assert.h>

	#include "../../discover.h"
	#include "../../proxy_handler.h"
	#include "../../proxy_structures.h"
	#include "../../service_cache.h"

	#include <ifaddrs.h>
	#include <net/if.h>
	#include "concom.h"

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

	#define PARTICLE_CONNECTION_TIMEOUT 10000
	//#define PARTICLE_CONNECTION_RESEND 1000

	#define PARTICLE_RETRY_TIMEOUT 200
	#define PARTICLE_RETRIES 10

	typedef struct
	{
		cc_pkt pkt;
		size_t paylen;
	} resendPacket;

	typedef struct
	{
		uint8_t id[8];
		double timeout;
	}particle_connection;

	int sock;
	struct sockaddr_in bcast;
	struct timeval curtimeval;
	resendPacket lastpkt;

	double curtimemilli = 0.0f;
	#define remote_device_get_particleId(rem_dev) ((particle_connection*)remote_device_get_addr(rem_dev))->id

	#define remote_device_get_connection(rem_dev) ((particle_connection*)remote_device_get_addr(rem_dev))

	static int has_particleId(void *c,void *id) {return (0==memcmp(&((particle_connection *)c)->id,id,8));}
	#define device_proxy_list_get_device_by_id(id) device_proxy_list_get_device(has_particleId,id)

	/* returns data tuple and fills out arguments on success, else NULL*/
	static cc_tuple *packet_getoperation(cc_pkt *p, uint8_t **pid,uint16_t *service,uint8_t *op)
	{
		uint16_t type;
		cc_tuple *c=&p->payload.first;

		*pid=p->header.srcid;

		type=CC32('S','V','C');

		if(!(
				   c->type==type
				&& c->len==sizeof(uint16_t)
				)) return NULL;
		else *service=ntohs(*(uint16_t*)c->buf);

		type=CC32('O','P','?');
		if(!(   (c=cc_tuple_next(c))
				&& c->type==type
				&& c->len==sizeof(uint8_t)
				)) return NULL;
		else *op=(*(uint8_t*)c->buf);

		if(!(   (c=cc_tuple_next(c))
				&& c->type==CC32('X','M','L')
				))	return NULL;
		else return c;
	}

	/*returns data tuple if matches, else NULL */

	static cc_tuple *packet_getmatching(cc_pkt *p, uint8_t *pid,uint16_t service,uint8_t op)
	{
		uint8_t *ppid;
		uint16_t pservice;
		uint8_t pop;
		cc_tuple *ret=packet_getoperation(p,&ppid,&pservice,&pop);
		if((	   ret
				&& 0==memcmp(pid,ppid,8)
				&& service==pservice
				&& op==pop)
				)
		  return ret;
		else
		 return NULL;
	}


	volatile int shutdownFlag_discovery = 0; /* Flag indicates systemshutdown. */

	void *discovery_worker_loop();

	int discovery_worker_init() {
		device_proxy_init_device_list();
		service_cache_init();

		{
			struct addrinfo hints={},*host_results;

			hints.ai_family = AF_INET; // IPv4
			hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
			hints.ai_flags = AI_PASSIVE;

			{
				int res,opt=1;
				// get ready to connect
				if ((res = getaddrinfo(NULL, "5555", &hints, &host_results)) != 0) {

					fprintf(
							stderr,
							"no idea what, but something went wrong setting up the address.%s",
							gai_strerror(res));
					return 1;
				}


				if ((sock = socket(host_results->ai_family, host_results->ai_socktype,
						host_results->ai_protocol)) == -1) {
					perror("socket");
					exit(1); // (-1) We are not able to discover, end of all days
				}

				if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
							sizeof(opt)) < 0) {
						perror("setsockopt REUSEADDR");
						exit(1);
				}

				if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &opt,
							sizeof(opt)) < 0) {
						perror("setsockopt REUSEADDR");
						exit(1);
				}

				if (bind(sock, host_results->ai_addr, host_results->ai_addrlen) == -1) {
						perror("bind");
						close(sock);
						return -1; // We are not able to discover, end of all days
				}

				{
					int flags = fcntl(sock, F_GETFL, 0) | O_NONBLOCK;
					if (fcntl(sock, F_SETFL, flags) == -1)
					{
						perror("nonblocking");
						exit (1);
					}
				}
                {
                        struct ifaddrs * ifap;

                        if (getifaddrs(&ifap) == 0)
                        {
                        	struct ifaddrs * p;
                                for( p = ifap;p;p=p->ifa_next)
                                {
                                	struct sockaddr *a=p->ifa_broadaddr;
                                	if(p->ifa_flags & IFF_RUNNING && a && a->sa_family==AF_INET)
                                	{
                                        memcpy(&bcast,a,sizeof(bcast));
                                        bcast.sin_addr.s_addr=htonl(INADDR_BROADCAST);
                                        bcast.sin_port=htons(5556);
                                        break;
                                	}
                                }
                                freeifaddrs(ifap);
                        }
                        freeaddrinfo(host_results);
                }
			}

		}

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


	struct cc_pkt_header cc_header = {CC_VERSION, };

	struct pkt_queue{
		cc_pkt p;
		struct pkt_queue *next;
	};

	static volatile cc_pkt *pkt_received;
	static pthread_cond_t  pkt_received_cond  = PTHREAD_COND_INITIALIZER;
	static pthread_mutex_t pkt_received_mutex = PTHREAD_MUTEX_INITIALIZER;


	void send_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
			struct soap* msg, u_char* buf, ssize_t len) {

		uint8_t *pid=0;

		size_t sent_len = 0;

		printf("sent service id %u", service_id);

		struct remote_device *rem_dev = get_device(msg);
		last_rem_dev = rem_dev;

		if (rem_dev == NULL) {
			printf("No device matching %s found.", wsa_header_get_To(msg));
			return;
		}

		pid = remote_device_get_particleId(rem_dev);

		assert(pid);

		{
			cc_pkt p={};
			cc_tuple *c=&p.payload.first;
			cc_header.seqnr++;
			memcpy(&p.header,&cc_header,sizeof(cc_header));

			c->type=CC32('S','V','C');
			c->len=sizeof(service_id);
			*(uint16_t *)c->buf=ntohs(service_id);
			c=cc_tuple_next(c);

			c->type=CC32('O','P','?');
			c->len=sizeof(uint8_t);
			*(uint8_t *)c->buf=op_id;
			c=cc_tuple_next(c);

			c->type=CC32('X','M','L');
			c->len=len;
			memcpy(c->buf,buf,c->len);
			c=cc_tuple_next(c);

			c->type=CC32('C','A','D');
			c->len=8;
			memcpy(c->buf,pid,c->len);

			memcpy(&lastpkt.pkt,&p,sizeof(p));
			lastpkt.paylen = cc_pkt_get_payload_len(&p,c);
			size_t len = cc_pkt_finalize(&p,c);
			printf("Paylen: %d Len: %d\n",lastpkt.paylen,len);
			sent_len = sendto(sock,&p,len,0,(struct sockaddr *)&bcast,sizeof(bcast));
		}

		printf("send op[%d,%d] to %s with length %lu\n", service_id, op_id,
				wsa_header_get_To(msg), (long int) sent_len);

		return;
	}

	void resendLastPkt()
	{
		ssize_t sent;
		cc_header.seqnr++;

		cc_pkt p={};
		memcpy(&p,&lastpkt.pkt,sizeof(p));
		memcpy(&p.header,&cc_header,sizeof(cc_header));
		//lastpkt.pkt.header.seqnr++;
		sent = sendto(sock,&p,cc_pkt_finalize2(&p,lastpkt.paylen),0,(struct sockaddr *)&bcast,sizeof(bcast));
		printf("Sent: %d\n",sent);
	}

	ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id,
			struct soap* msg, char **buf) {

		*buf = NULL;
		size_t buf_len = 0;


		//	struct remote_device *rem_dev = get_device(msg);
		//TODO: FIX generated API!!!
		struct remote_device *rem_dev = last_rem_dev;
		uint8_t *p_id;

		if (rem_dev == NULL) {
			printf("No device matching %s found.", wsa_header_get_To(msg));
			return -1;
		}

	   p_id=remote_device_get_addr(rem_dev);

	   pthread_mutex_lock(&pkt_received_mutex);
	   {
		   uint8_t retries = PARTICLE_RETRIES;
		   struct timeval tv;
		   struct timespec ts;
		   gettimeofday(&tv, NULL);

		   ts.tv_sec = tv.tv_sec + ((PARTICLE_RETRY_TIMEOUT/1000));
		   ts.tv_nsec = tv.tv_usec*1000 + ((PARTICLE_RETRY_TIMEOUT%1000)*1000000) ;
		   {
		   cc_tuple *c;

		   do{
			   int ret;

			   switch(ret=pthread_cond_timedwait(&pkt_received_cond,&pkt_received_mutex,&ts))
			   {
			    case ETIMEDOUT:
			    case EINVAL:
				   if(!retries)
				   {
					   buf_len = -1;
					   break;
				   } else
				   {
					   // Resend
					   resendLastPkt();

					   printf("Retrying last packet\n");

					   retries--;
					   gettimeofday(&tv, NULL);

					   ts.tv_sec = tv.tv_sec + ((PARTICLE_RETRY_TIMEOUT/1000));
					   ts.tv_nsec = tv.tv_usec*1000 + ((PARTICLE_RETRY_TIMEOUT%1000)*1000000) ;
				   }
				   break;
			    case 0:
			    	break;
			    default:
			    	   buf_len = -1;
			    	   fputs(strerror(ret),stderr);
			    	   break;
			   }


		   } while(!buf_len && (!pkt_received || !(c=packet_getmatching((cc_pkt*)pkt_received,p_id,service_id,op_id))));

		   if(!buf_len)
		   {
			   printf("Received %d bytes\n",c->len);
			   buf_len=c->len;
			   *buf=malloc(buf_len);
			   memcpy(*buf,c->buf,buf_len);
		   }
		   else
			   printf("Giving up\n");
		   }

	    pkt_received=NULL;
	   }
	    pthread_mutex_unlock(&pkt_received_mutex);

		return buf_len;
	}


	static int handle_node_message(cc_pkt *p) {
        if(p->payload.first.type!=CC32('S','V','C')) return 0;

		pthread_mutex_lock(&pkt_received_mutex);
		pkt_received=p;
		pthread_cond_broadcast(&pkt_received_cond);
		pthread_mutex_unlock(&pkt_received_mutex);

		if(pkt_received)
		{

			void (*deliver_event)(int service,int op, void * device, char * buffer, size_t buf_len);
			uint8_t *p_id;
			uint16_t svc_id;
			uint8_t op_id;


			cc_tuple *c=packet_getoperation(p,&p_id,&svc_id,&op_id);

			if(c)
			{
				struct remote_device *rem_device = device_proxy_list_get_device_by_id(p_id);
				struct dpws_s *device = rem_device ? remote_device_get_dpws_device(	rem_device) : NULL;
				if (!device) {
							printf("device for event not found");
							return -1;
						}

				get_event_func(&deliver_event,SSimpDevice);
				deliver_event(svc_id,op_id,device,c->buf,c->len);
			}


		}
		pthread_mutex_lock(&pkt_received_mutex);
		pkt_received=NULL;
		pthread_mutex_unlock(&pkt_received_mutex);
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


	/* process the information a node sends with the discovery message */
	static int discovery_process_pkt(cc_pkt *p) {
		struct remote_device *d;
		if((d=device_proxy_list_get_device_by_id(p->header.srcid)))
		{
			particle_connection *p=(particle_connection *) remote_device_get_addr(d);
			p->timeout=curtimemilli + PARTICLE_CONNECTION_TIMEOUT;
			//printf("Found known particle %d.%d.%d.%d.%d.%d.%d.%d\n",p->id[0],p->id[1],p->id[2],p->id[3],p->id[4],p->id[5],p->id[6],p->id[7]);
			return 0;
		}
		else
		{
		char *version = NULL, *model = "SSimp";
		char *serial_num = NULL;
		int result;

		uint8_t *id=p->header.srcid;
		asprintf(&serial_num,"%d.%d.%d.%d.%d.%d.%d.%d",id[0],id[1],id[2],id[3],id[4],id[5],id[6],id[7]);
		asprintf(&version,"%d.%d",id[0],id[1]);

		printf("particle %s found\n",serial_num);
		fflush(stdout);
		particle_connection addr;
		memcpy(addr.id,id,sizeof(addr.id));
		addr.timeout=curtimemilli + PARTICLE_CONNECTION_TIMEOUT;

		result = dpws_device_build_and_register(model,&addr,sizeof(addr),
							gateway_get_interface(),version, serial_num, serial_num);
		printf("particle %s registered\n",serial_num);
		fflush(stdout);

		free(version);
		free(serial_num);

		return result;
		}
	}

	static void ping(char *pid)
	{
		{
					cc_pkt p={};
					cc_tuple *c=&p.payload.first;
					cc_header.seqnr++;
					memcpy(&p.header,&cc_header,sizeof(cc_header));

					c->type=CC32('A','C','M');
					c->len=0;
					c=cc_tuple_next(c);

					c->type=CC32('C','H','E');
					c->len=8;
					memcpy(c->buf,pid,c->len);

					sendto(sock,&p,cc_pkt_finalize(&p,c),0,(struct sockaddr *)&bcast,sizeof(bcast));
				}
	}

	int has_timedout(void *in,void *dummy)
	{
		particle_connection *p=(particle_connection*) in;

		//if(0==p->timeout/PARTICLE_CONNECTION_RESEND) ping(p->id);

		return p->timeout < curtimemilli;
	}

	void updateCurTime()
	{
		gettimeofday(&curtimeval,NULL);
		curtimemilli = curtimeval.tv_sec * 1000.0f + curtimeval.tv_usec / 1000.0f;
	}

	void *discovery_worker_loop() {
		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(sock,&read_fds);
		updateCurTime();


		while(gateway_running())
		{
			struct timeval timeout={0,13000};
			cc_pkt p={};
			int ret;
			select(sock,&read_fds,NULL,NULL,&timeout);
			if(0<(ret=recv(sock,&p,sizeof(p),0))) //TODO acknowledge length
			{
			discovery_process_pkt(&p);
			handle_node_message(&p);
			}
			{
					  updateCurTime();

					  struct remote_device *d;
					  while((d=device_proxy_list_get_device(has_timedout,NULL)))
					  {
						   particle_connection *p=(particle_connection *)remote_device_get_addr(d);
						   printf("particle %d.%d.%d.%d.%d.%d.%d.%d timed out\n",p->id[0],p->id[1],p->id[2],p->id[3],p->id[4],p->id[5],p->id[6],p->id[7]);
						   fflush(stdout);
						   dpws_device_unregister(d);
						   printf("particle %d.%d.%d.%d.%d.%d.%d.%d removed\n",p->id[0],p->id[1],p->id[2],p->id[3],p->id[4],p->id[5],p->id[6],p->id[7]);
						   fflush(stdout);
					  }
			}
		}
		close(sock);
		printf("\nDiscovery: Listened long enough\n");
		fflush(stdout);
		device_proxy_list_cleanup();
		printf("\nDiscovery:exiting\n");
		fflush(stdout);
		return NULL;
	}

	void discovery_notify_unregister(struct remote_device *rem_device)
	{
		particle_connection *p=(particle_connection *)remote_device_get_addr(rem_device);
		printf("deleted particle %d.%d.%d.%d.%d.%d.%d.%d",p->id[0],p->id[1],p->id[2],p->id[3],p->id[4],p->id[5],p->id[6],p->id[7]);
	}
