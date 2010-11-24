/*************************************
*	serial.cpp
*	serial.cpp is the implementation file to the serial
*	port functions.  This file contains all the information for
*	opening, closing, reading, writing, and purging the
*	communications ports.  Please, see each function as well
*	as the Microsoft MSDN documentation for extended information
*	on function calls.
*************************************/



#include "serial.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>





/*********************
*	port_open
*	port_open opens the serial port given by lpszPort.  The port is opened
*	with the following specifications:
*	Baud: 115200
*	Byte Size: 8
*	Parity:	None
*	Stop Bits:	1
*	Read Timeout Between Characters: 100 ms
*	Input Buffer Size: 2048 bytes
*	Output Buffer Size: 2048 bytes
*	I/O; Synchronous
*	NOTE: The format of the incoming string must be that of the following:
*				\\.\COMX where X is some number
*	The standard COMX format will work only for ports 9 and below.  This issue
*	is documented in the the Microsoft MSDN Knowledge Base article Q115831 available
*	at http://support.microsoft.com/default.aspx?scid=kb;en-us;115831
*	PRE:	lpszPort is an LPCSTR type with the format listed in the note above
*	POST:	if successful, the handle to the port signified by lpszPort has been returned
*			if failed, NULL is returned
*********************/
int port_open(/* in */ const char * device)
{

	int handle = open(addr,O_RDWR|O_NOCTTY| O_NONBLOCK| O_SYNC);
	if(handle==-1)
	{
		perror(NULL);
		return handle;
	}
	fcntl (handle, F_SETFL, O_ASYNC);
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
	return handle;
}

/*********************
*	port_close
*	port_close takes in the handle of a port signified by handle
*	and attempts to close that port.
*	PRE:	handle is a legitimate port HANDLE
*	POST:	if successful, 0 has been returned
*			if failed, -1 has been returned
*********************/
int port_close(/* in */ int handle)
{
	int ret = close(handle);
	if(ret==-1)
	{
		perror(NULL);
	}

	return ret;
}

/*********************
*	port_purge
*	port_purge takes in the handle of a port signified by handle and
*	purges the input and output buffers if they exist.
*	PRE:	handle is a legitimate port HANDLE
*	POST:	if successful, 0 has been returned
*			if failed, -1 has been returned
*********************/
int port_flush(/* in */ int handle)
{
	tcflush(handle,TCIOFLUSH);
	return 0;
}

/*********************
*	port_write
*	port_write takes in a the handle of a port signified by handle and attempts
*	to write data to that port.
*	PRE:	handle is a legitimate port HANDLE
*			ucCmd is a pointer to an unsigned char array.  This array is not NULL terminated
*			dwLength is the length of ucCMD of type DWORD
*			dwBytes_written will hold the number of bytes written to the serial port after WriteFile
*				has been called.
*			iPos is a legitimate integer
*	POST:	iPos holds the line position that any data was written to in the console.
*			if successful, 0 has been returned and dwBytes_written holds the number of bytes written
*				to the serial port.
*			if failed, -1 has been returned and dwBytes_written holds the number of bytes written 
*				to the serial port.
*********************/
int port_write(	/* in */ int handle, /* in */ unsigned char *ucCmd,
				/* in */ int length)
{



	port_flush(handle);
	int bytes = write(handle,ucCmd,length);
	if(bytes==-1)
	{
		perror(NULL);
		return -1;
	}

	return bytes;
}

/*********************
*	port_read
*	port_read takes in the handle of the port signified by handle and attempts to read
*	data from that port.
*	PRE:	handle is a legitimate port HANDLE
*			ucResult is a poitner to an unsigned char array.  This array is written to, but
*				memory must be allocated by the user before the function call.
*			dwLength holds the number of bytes to read from the serial port.  This value should
*				be the length of ucResult.
*			dwBytes_written will hold the number of bytes read from the serial port after
*				ReadFile is called
*			iPos is a legitimate integer
*	POST:	iPos holds the line position that any data was written to in the console.
*			if successful, 0 has been returned and dwBytes_read holds the number of bytes read
*				from the serial port.
*			if failed, -1 has been returned and dwBytes_read holds the number of bytes read 
*				from the serial port.
*********************/
int port_read(	/* in */ int handle, /* in/out */ unsigned char *ucResult,
				/* in */ int length)
{

	int bytes = read(handle,ucResult,length);
	if(bytes==-1)
	{
		perror(NULL);
		return -1;
	}

	return bytes;

}
