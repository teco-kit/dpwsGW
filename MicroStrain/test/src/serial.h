/*********************
*	serial.h
*	seroa;.h holds the declarations for each individual
*	function that applies to the seroa; [prt.  For a better understanding
*	of each function, please, see the implementation file, serial.cpp
*	and reference the desired function.
*********************/

#ifndef	SERIAL_H
#define	SERIAL_H

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
int port_open(/* in */ const char * device);

/*********************
*	port_close
*	port_close takes in the handle of a port signified by hPort
*	and attempts to close that port.
*	PRE:	hPort is a legitimate port HANDLE
*	POST:	if successful, 0 has been returned
*			if failed, -1 has been returned
*********************/
int port_close(/* in */ int handle);

/*********************
*	port_purge
*	port_purge takes in the handle of a port signified by hPort and
*	purges the input and output buffers if they exist.
*	PRE:	hPort is a legitimate port HANDLE
*	POST:	if successful, 0 has been returned
*			if failed, -1 has been returned
*********************/
int port_flush(/* in */ int handle);

/*********************
*	port_write
*	port_write takes in a the handle of a port signified by hPort and attempts
*	to write data to that port.
*	PRE:	hPort is a legitimate port HANDLE
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
				/* in */ int length);

/*********************
*	port_read
*	port_read takes in the handle of the port signified by hPort and attempts to read
*	data from that port.
*	PRE:	hPort is a legitimate port HANDLE
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
				/* in */ int length);

#endif
