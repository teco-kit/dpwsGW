/*********************
*	node.h
*	node.h holds the declarations for each individual
*	function that applies to the nodes.  For a better understanding
*	of each function, please, see the implementation file, node.cpp
*	and reference the desired function.
*********************/
#ifndef	NODE_H
#define	NODE_H

/***********************
*	nd_ping
*	nd_ping attempts to ping the node address, usNode, over the port, hPort.
*	Command Format
*		Length:		3 bytes
*		Byte 1:		0x02
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x02
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_ping(/* in */ int handle,
			/* in */ unsigned short usNode);

/***********************
*	nd_ping_long
*	nd_ping_long attempts to ping the node address, usNode, over the port, hPort.
*	Command Format
*		Length:		10 bytes
*		Byte 1:		0xAA		SOP
*		Byte 2:		0x05		Message Type
*		Byte 3:		0x00		Address Mode
*		Byte 4:		MSB of the node's address
*		Byte 5:		LSB of the node's address
*		Byte 6:		0x02
*		Byte 7:		0x00
*		Byte 8:		0x02
*		Byte 9:		MSB of the checksum
*		Byte 10:	LSB of the checksum
*	Good Response:
*		Length:		13 Bytes

*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_ping_long(/* in */ int handle,
					/* in */ unsigned short usNode);

/***********************
*	nd_eeprom_read
*	nd_eeprom_read attempts to read an eeprom location, usLocation, from the node address, usNode,
*	over the port given by handle, hPort
*	Command Format
*		Length:		5 bytes
*		Byte 1:		0x03
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*		Byte 4:		MSB of the location to read
*		Byte 5:		LSB of the location to read
*	Good Response:
*		Length:		5 byte
*		Byte 1:		0x03
*		Byte 2:		MSB of read value
*		Byte 3:		LSB of read value
*		Byte 4:		MSB of checksum
*		Byte 5:		LSB of checksum
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to read
*			usLocation is an unsigned short holding the location to read
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_eeprom_read(/* in */ int handle,
					/* in */ unsigned short usNode,
					/* in */ unsigned short usLocation,
					/* out */ unsigned short & usResponse);

/***********************
*	nd_eeprom_write
*	nd_eeprom_write attempts to write a value, usValue, to an EEPROM location, usLocation, on a node,
*	usNode, over the port opened by hPort.
*	Command Format
*		Length:		8 bytes
*		Byte 1:		0x04
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*		Byte 4:		location to write
*		Byte 5:		MSB of value to write
*		Byte 6:		LSB of value to write
*		Byte 7:		MSB of checksum
*		Byte 8:		LSB of checksum
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x04
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to write
*			usLocation is an unsigned short holding the location to write
*			usValue is an unsigned short holding the value to write
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_eeprom_write(/* in */ int handle,
					/* in */ unsigned short usNode,
					/* in */ unsigned char ucLocation,
					/* in */ unsigned short usValue);

/***********************
*	nd_page_download
*	nd_page_download attempts to download a page, usPage, on a node, usNode, over the port
*	opened by hPort.
*	Command Format
*		Length:		5 bytes
*		Byte 1:		0x05
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*		Byte 4:		MSB of page to download
*		Byte 5:		LSB of page to download
*	Good Response:
*		Length:		267 bytes
*		Byte 1:		0x05
*		Byte 2-265:	Incoming data
*		Byte 266:	MSB of checksum
*		Byte 267:	LSB of checksum
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to download from
*			usPage is an unsigned short holding the page number to download
*			usResponse is an array of length 132 that will hold all the incoming data
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_page_download(/* in */ int handle,
					 /* in */ unsigned short usNode,
					 /* in */ unsigned short usPage,
					 /* out */ unsigned short usResponse[]);

 /***********************
*	nd_trigger
*	nd_trigger triggers a node, usNode, with an id value, ucID, over the port
*	opened by hPort
*	Command Format
*		Length:		4 bytes
*		Byte 1:		0x0C
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*		Byte 4:		Trigger ID value
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x0C
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to download from
*			ucID is the ID value for the trigger
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_trigger(/* in */ int handle,
				/* in */ unsigned short usNode,
				/* in */ unsigned char ucID);

/***********************
*	nd_sleep
*	nd_sleep puts a node, usNode, to sleep.
*	NOTE: The wake command is a ping command sent continuously.  This can be accomplished
*			with a for, while, do while, etc. loop and checking the response on each ping
*			attempt.
*	Command Format
*		Length:		3 bytes
*		Byte 1:		0x32
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*	Good Response:
*		None
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to sleep
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_sleep(/* in */ int handle,
			 /* in */ unsigned short usNode);

/***********************
*	nd_erase
*	nd_erase erases a node, usNode.
*	Command Format
*		Length:		7 bytes
*		Byte 1:		0x32
*		Byte 2:		MSB of the node's address
*		Byte 3:		LSB of the node's address
*		Byte 4:		0x08
*		Byte 5:		0x10
*		Byte 6:		0x0C
*		Byte 7:		0xFF
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x06
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to erase
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_erase(/* in */ int handle,
				/* in */ unsigned short usNode);


/***********************
*	nd_stream_init
*	nd_stream_init initiates a stream on a node, usNode, with an 8 bit checksum.
*	Command Format
*		Length:		3
*		Byte 1:		0x38
*		Byte 2:		MSB of Node Address
*		Byte 3:		LSB of Node Address
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x06
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to initiate a stream on
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_stream_init(/* in */ int handle,
					/* in */ unsigned short usNode);

/***********************
*	nd_stream_sweep
*	nd_stream_sweep retrieves a sweep from the streaming node via the port opened by
*	the handle, hPort.
*	Command Format
*		Length:		0
*	Good Response:
*		Length:			Num Channels * 2 + 1 byte checksum + 1 byte next sweep flag
*		Byte 1:			MSB of 1st channel's data
*		Byte 2:			LSB of 1st channel's data
*		Byte 3:			MSB of 2nd channel's data
*		...
*		Byte N - 2:		Checksum of bytes 1 through N - 3
*		Byte N - 1:		Next sweep flag
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			pusResponse is a legitimate pointer of length equal to the number of active
*				channels on the streaming node signified by ucNum_channels which holds
*				the sweep returned by the serial port
*			ucNum_channels is an unsigned character that holds the number of active channels
*				on the streaming node.  This is also equal to the length of pusResponse.
*	POST:	if successful, 0 is returned and pusResponse holds the next sweep in the stream
*			otherwise, some error code
***********************/
int nd_stream_sweep(/* in */ int handle,
					/* in/out */ unsigned short *pusResponse,
					/* in */ unsigned char ucNum_channels);

/***********************
*	nd_low_duty_cycle_init
*	nd_low_duty_cycle_init attempts to initiate a low duty cycle on a node, usNode, over the port, hPort.
*	Command Format
*		Length:		10 bytes
*		Byte 1:		0xAA		SOP
*		Byte 2:		0x05		Message Type
*		Byte 3:		0x00		Address Mode
*		Byte 4:		MSB of Node Address
*		Byte 5:		LSB of Node Address
*		Byte 6:		0x02
*		Byte 7:		0x00		Command Byte
*		Byte 8:		0x38		Command Byte
*		Byte 9:		MSB Checksum
*		Byte 10:	LSB Checksum
*	Good Response:
*		Length:		13 Bytes
*		Byte 1:		0xAA		Valid packet response from base station
*		Byte 2:		0xAA		SOP
*		Byte 3:		0x05		Message Type
*		Byte 4:		0x00		Address Mode
*		Byte 5:		MSB of Node Address
*		Byte 6:		LSB of Node Address
*		Byte 7:		0x02
*		Byte 8:		0x00
*		Byte 9:		0x00
*		Byte 10:	0xXX		LQI
*		Byte 11:	0xXX		RSSI
*		Byte 12:	MSB Checksum
*		Byte 13:	LSB Checksum
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_low_duty_cycle_init(/* in */ int handle,
						   /* in */ unsigned short usNode);

/***********************
*	nd_low_duty_cycle_packet
*	nd_low_duty_cycle_packet attempts to receive a low duty cycle from a node, usNode, over the port, hPort.
*	Good Response:
*		Length:			XX Bytes
*		Byte 1:			0xAA		SOP
*		Byte 2:			0x07		Message Type
*		Byte 3:			0x04		Address Mode
*		Byte 4:			0xXX		MSB Received From Address
*		Byte 5:			0xXX		LSB	Received From Address
*		Byte 6:			0xXX		Packet Length (does not include LQI or RSSI)
*		Byte 7:			0x02		Application identifier
*		Byte 8:			0xXX		Channel Mask
*		Byte 9:			0xXX		Sampling rate of the packet
*		Byte 10:		0x01		Data Type - Always 2 byte integer
*		Byte 11:		0xXX		MSB Timer Tick
*		Byte 12:		0xXX		LSB Timer Tick
*		Byte 13:		0xXX		MSB of first channel of data
*		Byte 14:		0xXX		LSB of first channel of data
*		Byte 15:		to #						MSB/LSB of the rest of the channels of data if there are any.  Thus, the total
*						channels *					number of bytes to read of channel data is the number of channels active
*						2 minutes 3	0xXX			in the channel mask * 2.
*		
*		Byte N - 3:		0xXX		LQI
*		Byte N - 2:		0xXX		RSSI
*		Byte N - 1:		0xXX		Packet checksum MSB
*		Byte N:			0xXX		Packet checksum LSB
*
*	0xXX - Variable value
*	Checksum = Sum of bytes 2 through N - 4 minus the LQI and RSSI = ((Byte N) * 256 + Byte N - 1) & 0xFFFF
*	The LQI and RSSI are not part of the checksum
***********************/
int nd_low_duty_cycle_packet(/* in */ int handle,
							 /*	in	*/ unsigned char ucNumChannels);

/***********************
*	nd_high_speed_stream_init
*	nd_high_speed_stream_init attempts to initiate a high speed stream on a node, usNode, over the port, hPort.
*	Command Format
*		Length:		10 bytes
*		Byte 1:		0xAA		SOP
*		Byte 2:		0x05		Message Type
*		Byte 3:		0x00		Address Mode
*		Byte 4:		MSB of Node Address
*		Byte 5:		LSB of Node Address
*		Byte 6:		0x02
*		Byte 7:		0x00		Command Byte
*		Byte 8:		0x39		Command Byte
*		Byte 9:		MSB Checksum
*		Byte 10:	LSB Checksum
*	Good Response:
*		Length:		1 Byte
*		Byte 1:		0xAA		Valid packet response from base station
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_high_speed_stream_init(/* in */ int handle,
							  /* in */ unsigned short usNode);

/***********************
*	nd_high_speed_stream_packet
*	nd_high_speed_stream_packet attempts to receive a high speed packet from a node, usNode, over the port, hPort.
*	Good Response:
*		Length:			XX Bytes
*		Byte 1:			0xAA		SOP
*		Byte 2:			0x01		Message Type
*		Byte 3:			0x05		Address Mode
*
*		Byte 4:			0xXX		MSB Received From Address
*		Byte 5:			0xXX		LSB	Received From Address
*
*		Byte 6:			0xXX		Packet Length (does not include LQI or RSSI)
*		Byte 7:			0x02		Application identifier
*		Byte 8:			0xXX		Channel Mask   
*		Byte 9:			0xFF		Stream rate of the packet, fixed at 0xFF, the actual rate is dermined by the number of channels
*		Byte 10: 		0xXX		Number of samples
*
*		Byte 11:		0xXX		Timer Value (4 bytes)
*		Byte 12:		0xXX		
*		Byte 13:		0xXX		
*		Byte 14:		0xXX		
*
*       Byte 15 to N-4: 0xXX       Sample Data 
*		for each sample
*		   for each channel
*		      Byte 15 + (S*Channels + C)*2: MSB of Sample(S) channel(C)
*			  Byte 16 + (S*Channels + C)*2:	LSB of Sample(S) channel(C)
*		
*		Byte N - 3:		0xXX		LQI
*		Byte N - 2:		0xXX		RSSI
*
*		Byte N - 1:		0xXX		Packet checksum MSB
*		Byte N:			0xXX		Packet checksum LSB
*
*	0xXX - Variable value
*	Checksum = Sum of bytes 2 through N - 4(exclude LQI, RSSI, and cheksum) = ((Byte N) * 256 + Byte N - 1) & 0xFFFF
*	The LQI and RSSI are not part of the checksum
***********************/
int nd_high_speed_stream_packet(/* in */ int handle);


/***********************
*	nd_discovery_packet
*	nd_discovery_packet attempts to receive a node discovery packet from any node that sends one over the port, hPort.
*	Good Response:
*		Length:		1 Byte
*		Byte 1:		0xAA		SOP
*		Byte 2:		0x00		Message Type
*		Byte 3:		0x00		Address Mode
*		Byte 4:		0xXX		MSB Received From Address
*		Byte 5:		0xXX		LSB	Received From Address
*		Byte 6:		0x03		Packet Length
*		Byte 7:		0xXX		Radio Channel
*		Byte 8:		0xXX		MSB of Model Number (see node EEPROM map for valid model numbers in EEPROM 112)
*		Byte 9:		0xXX		LSB of Model Number (see node EEPROM map for valid model numbers in EEPROM 112)
*		Byte 10:	0xXX		LQI
*		Byte 11:	0xXX		RSSI
*		Byte 12:	0xXX		Packet checksum MSB
*		Byte 13:	0xXX		Packet checksum LSB
*
*	0xXX - Variable value
*	Checksum = Sum of bytes 2 through 9 = ((Byte N) * 256 + Byte N - 1) & 0xFFFF
*	The LQI and RSSI are not part of the checksum
***********************/
int nd_discovery_packet(/* in */ int handle,
						/* in */ int iTimeout);


/***********************
*	nd_kill_node
*	nd_kill_node attempts to stop a node that is in a state that doesn't accept commands. ie streaming, LDC etc
*	Command Format
*	Command Format
*		Length:		10 bytes
*		Byte 1:		0xAA		SOP
*		Byte 2:		0xFE		Message Type
*		Byte 3:		0x00		Address Mode
*		Byte 4:		MSB of Node Address
*		Byte 5:		LSB of Node Address
*		Byte 6:		0x02
*		Byte 7:		0x00		Command Byte
*		Byte 8:		0x90		Command Byte
*		Byte 9:		MSB Checksum
*		Byte 10:	LSB Checksum
*	Good Response:
*		Length:		1 Byte
*		Byte 1:		0x90		Valid packet response from base station
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to stop
*	POST:	if command is sent successful, 0 is returned
*			otherwise, some error code
*
* Note:Must call nd_kill_node_status to poll the status of the kill node command
* you can also call abort the kill node command by sending any byte to the
* basestation. Pinging the basestation will suffice.
***********************/
int nd_kill_node_init(/* in */ int handle,
					  /* in */ unsigned short usNode);


/***********************
*	nd_kill_node_status
*	nd_kill_node_status waits 100ms for a response from the basestation. 
*	Response:
*		Length:		1 Byte
*		Byte 1:		0x90		The node was successfully stop and is now communicating
*       		    0x21		The kill command was aborted
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			nd_kill_node_init was previously called
*	POST:	if command is sent successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_kill_node_status(/* in */ int handle,
					    /* out */ bool& bNodeKilled);

int nd_kill_node_abort(/* in */ int handle);

#endif
