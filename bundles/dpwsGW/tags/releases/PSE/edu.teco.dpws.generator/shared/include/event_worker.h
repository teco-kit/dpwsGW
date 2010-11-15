#ifndef EVENT_WORKER_H_
#define EVENT_WORKER_H_

int
event_worker_init (void *device,uint8_t *);

int
event_worker_shutdown ();
char * get_global_packet_buf(void);


#endif /*EVENT_WORKER_H_*/
