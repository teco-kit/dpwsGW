/*
 * operations.c
 *
 *  Created on: 02.03.2010
 *      Author: riedel
 */
#include <bitsio/write_bits_buf.h>
#include <bitsio/read_bits_buf.h>

#include <Sample_dom2bin.h>
#include <StatusControl_bin2dom.h>
#include <Status_dom2bin.h>

#include <sys/socket.h>

#include <SensorValues_operations.h>
#include "sensorvalues_service.h"

typedef union {
 struct	{
		 uint16_t svc;
		 uint8_t op;
		 uint8_t len;
		} msg;
	char buf[4];
} header;

int send_event(int com_fd,uint16_t svc, uint8_t op)
{
	static uint16_t event=0;
	char sendbuf[256];
	struct WRITER_STRUCT* writer = write_bits_bufwriter_stack_new(
			sendbuf,sizeof(sendbuf));

	switch(op){
	case OP_SensorValues_SensorValuesEvent:
	{
	sens_SSimpSample s={};

	if(get_sensor_values(&s,++event%(1<<15))) return 0;

	Sample_dom2bin_run(&s, writer);
	{
		header h={{0,OP_SensorValues_SensorValuesEvent,write_buf_finish(writer)}};

		size_t sent_len;

		if ( send(com_fd, &h,sizeof(h),0)!=sizeof(h) ||
				(sent_len = send(com_fd, sendbuf, h.msg.len, 0))!=h.msg.len) {
			perror("send");
			return 2;
		}

		if (sent_len != h.msg.len) {
			printf("send_events: the message length sent (%d)"
				"differs from the buffer size (%d)", sent_len, h.msg.len);
		}
	}
	}break;
    }
	return 0;
}


ssize_t send_msg(int com_fd, uint16_t svc, uint8_t op, char *sendbuf, size_t len)
{
	header h={{svc,op,len}};
    size_t sent_len;

		if (    send(com_fd, &h,sizeof(h),0)!=sizeof(h) ||
				(sent_len = send(com_fd, (void *) sendbuf,h.msg.len, 0)) != h.msg.len) {
			perror("send");
		    return -1;
		}
		return sent_len;
}

int handle_request(int com_fd)
{
    header in;

   char sendbuf[256];
    struct WRITER_STRUCT* writer = write_bits_bufwriter_stack_new(
			sendbuf,sizeof(sendbuf));

	if (recv(com_fd, in.buf, 4, 0) != sizeof(in))
		return 1;

	{
		char *in_arg=alloca(in.msg.len);
		if(in.msg.len)
			if(in.msg.len!=recv(com_fd,in_arg,in.msg.len,0))
			{
				return 1;
			}

		switch(in.msg.svc)
		{
		case 0:
		switch(in.msg.op)
		{
		case OP_SensorValues_GetSensorValues: {
					sens_SSimpSample s={};

			    get_sensor_values(&s,-1);

			    Sample_dom2bin_run(&s, writer);

			    {
			    	size_t len=write_buf_finish(writer);
			    	if(send_msg(com_fd,in.msg.svc,in.msg.op,sendbuf,len)!=len)
			    		return 2;
				}
				}break;
		case OP_SensorValues_Config:
		{
			sens_SSimpControl ctrl={};
			sens_SSimpStatus ret={};

			struct READER_STRUCT* reader=read_bits_bufreader_stack_new(in_arg,in.msg.len);

			StatusControl_bin2dom_run(reader,&ctrl);

			configure_sensors(&ctrl,&ret);

			Status_dom2bin_run(&ret, writer);
			{
			    	size_t len=write_buf_finish(writer);
			    	if(send_msg(com_fd,in.msg.svc,in.msg.op,sendbuf,len)!=len)
			    		return 2;

			}
		}
		}
	}}
	return 0;
}
