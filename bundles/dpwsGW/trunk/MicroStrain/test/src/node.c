/*********************
*	node.cpp
*	node.cpp holds the implementation for each individual
*	function that applies to the nodes.  For a better understanding
*	of each function, please, reference the desired function.
*********************/

#include "node.h"

/*	used for serial (comm) port functionality	*/
#include "serial.h"



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
			/* in */ unsigned short usNode)
{
	TRACE("nd_ping()\n");
	unsigned char	ucCmd[3] =	{	0x02,	//	ping command byte
									usNode >> 8,	//	MSB of the node address
									usNode & 0xFF	//	LSB of the node address
								};
	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 3, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to ping node %i\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x", usNode, ucCmd[0], ucCmd[1], ucCmd[2]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	the only legitimate response is a 0x02	*/
	if (ucResponse != 0x02) {
		/*	append to the command description	*/
		strFormat.Format("Failed to ping node %i\r\nResponse: %#04x", usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Ping attempt successful for node %i\r\nResponse: %#04x", usNode, ucResponse);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
*		Byte 1:		0xAA		Valid packet response from base station
*		Byte 2:		0xAA		SOP
*		Byte 3:		0x07		Message Type
*		Byte 4:		0x02		Address Mode
*		Byte 5:		MSB of the node's address
*		Byte 6:		LSB of the node's address
*		Byte 7:		0x02
*		Byte 8:		0x00
*		Byte 9:		0x00
*		Byte 10:	0xXX		LQI
*		Byte 11:	0xXX		RSSI
*		Byte 12:	MSB of the checksum
*		Byte 13:	LSB of the checksum
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_ping_long(/* in */ int handle,
					/* in */ unsigned short usNode)
{
	TRACE("nd_ping_long()\n");
	unsigned char ucCmd[10] = {	0xAA,			//	SOP
								0x05,			//	Message Type
								0x00,			//	Address Mode
								usNode >> 8,	//	MSB of the node's address
								usNode & 0xFF,	//	LSB of the node's address
								0x02,			//	packet length
								0x00,			
								0x02,
								(0x09 + (usNode >> 8) + (usNode & 0xFF)) >> 8,		//	MSB of the checksum
								(0x09 + (usNode >> 8) + (usNode & 0xFF)) & 0xFF		//	LSB of the checksum
	};

	unsigned char ucResponse[13];		//	holds the response
	DWORD	dwBytes_written = 0,		//	number of bytes written to the port
			dwBytes_read = 0;			//	number of bytes read from the port
	int iResult,						//	return value of port read/write functions
		iPos;							//	position in the console that the command was written to
	CString strFormat;					//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 10, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to ping node %i\r\nSOP: %#04x\t\t\tMessage Type: %#04x\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tMSB of Command Byte: %#04x\tLSB of Command Byte: %#04x\r\nMSB of Checksum: %#04x\t\tLSB of Checksum: %#04x",
			usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6], ucCmd[7],
			ucCmd[8], ucCmd[9]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, ucResponse, 13, dwBytes_read, iPos)) != 0) {
		if (iResult == ERROR_MISMATCHING_BYTE_LENGTH) {
			strFormat.Format("Failed to ping node %i", usNode);
			CString strTemp;
			for (DWORD i = 0; i < dwBytes_read; i++) {
				if (i % 3 == 0)
					strFormat += "\r\n";
				strTemp.Format("Response Byte %i: %#04x\t\t", i, ucResponse[i]);
				strFormat += strTemp;
			}
			strCmd_info[iPos] = strFormat;
		}
		return iResult;
	}

	/*	make sure the first two bytes are 0xAA and the packet is 13 bytes long, otherwise, the packet is bad.	*/
	if (dwBytes_read != 13 || ucResponse[0] != 0xAA || ucResponse[1] != 0xAA) {
		/*	append to the command description	*/
		strFormat.Format("Failed to ping node %i", usNode);
		CString strTemp;
		for (DWORD i = 0; i < dwBytes_read; i++) {
			if (i % 3 == 0)
				strFormat += "\r\n";
			strTemp.Format("Response Byte %i: %#04x\t\t", i, ucResponse[i]);
			strFormat += strTemp;
		}
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Ping attempt successful for node %i\r\nBase Station Response: %#04x\tSOP: %#04x\t\t\tMessage Type: %#04x\r\nAddress Mode: %#04x\t\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tMSB of Command Byte: %#04x\tLSB of Command Byte: %#04x\r\nLQI: %#04x\t\t\tRSSI: %#04x\r\nMSB of Checksum: %#04x\t\tLSB of Checksum: %#04x", usNode,
		ucResponse[0], ucResponse[1], ucResponse[2], ucResponse[3], ucResponse[4], ucResponse[5], ucResponse[6], ucResponse[7], ucResponse[8], ucResponse[9], ucResponse[10], ucResponse[11], ucResponse[12]);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
*		Byte 2:		MSB of read valuew
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
					/* out */ unsigned short & usResponse)
{
	TRACE("nd_eeprom_read()\n");
	unsigned char	ucCmd[5] =	{	0x03,	//	eeprom read command byte
									usNode >> 8,	//	MSB of the node address
									usNode & 0xFF,	//	LSB of the node address
									usLocation >> 8,	//	MSB of the location to read
									usLocation & 0xFF	//	LSB of the location to read
								};
	unsigned char ucResponse[5];		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 5, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to read EEPROM location %i on node %i\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nMSB EEPROM Location: %#04x\tLSB EEPROM Location: %#04x",
		usLocation, usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, ucResponse, 5, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	calculate the checksum
	*	checksum = sum of response bytes minus command byte and 2 checksum bytes	*/
	if (((ucResponse[1] + ucResponse[2]) & 0xFFFF) != (ucResponse[3] << 8) + ucResponse[4]) {
		/*	append to the command description	*/
		strFormat.Format("A bad checksum was calculated on the read from node %i\r\nReturned Checksum: %#04x\r\nCalculated Checksum: %#04x", usNode, (ucResponse[3] << 8) + ucResponse[4], (ucResponse[1] + ucResponse[2]) & 0xFFFF);
		strCmd_info[iPos] = strFormat;
		return ERROR_BAD_CHECKSUM;
	}

	/*	store the response	*/
	usResponse = (ucResponse[1] << 8) + ucResponse[2];

	/*	append to the command description	*/
	strFormat.Format("Successful read attempt on EEPROM location %i on node %i\r\nRead Value: %i\r\nChecksum: %#04x", 
		usLocation, usNode, usResponse, (ucResponse[3] << 8) + ucResponse[4]);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
					/* in */ unsigned short usValue)
{
	TRACE("nd_eeprom_write()\n");
	unsigned char	ucCmd[8] =	{	0x04,			//	eeprom write command byte
									usNode >> 8,	//	MSB of the node address
									usNode & 0xFF,	//	LSB of the node address
									ucLocation,		//	location to write
									usValue >> 8,	//	MSB of value to write
									usValue & 0xFF,	//	LSB of value to write
									0x00,			//	MSB of the checksum
									0x00			//	LSB of the checksum
								};
	/*	calculate the checksum	*/
	unsigned short usChecksum = (ucCmd[1] + ucCmd[2] + ucCmd[3] + ucCmd[4] + ucCmd[5]) & 0xFFFF;
	ucCmd[6] = usChecksum >> 8;		//	set the MSB of the checksum
	ucCmd[7] = usChecksum & 0xFF;	//	set the LSB of the checksum
	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 8, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to write %i to EEPROM location %i on node %i\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nEEPROM Location: %#04x\r\nMSB Write Value: %#04x\tLSB Write Value: %#04x\r\nMSB Checksum: %#04x\tLSB Checksum: %#04x", 
		usValue, ucLocation, usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6], ucCmd[7]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	if (ucResponse != 0x04) {
		/*	append to the command description	*/
		strFormat.Format("Failed write command with a bad response.\r\nResponse: %#04x", ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_BAD_CHECKSUM;
	}

	/*	append to the command description	*/
	strFormat.Format("Successful write attempt to write %i to EEPROM location %i on node %i", usValue, ucLocation, usNode);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
					 /* out */ unsigned short usResponse[])
{
	unsigned char	ucCmd[5] =	{	0x05,			//	page download command byte
									usNode >> 8,	//	MSB of node address
									usNode & 0xFF,	//	LSB of node address
									usPage >> 8,	//	MSB of page to download
									usPage & 0xFF	//	LSB of page to download
								};
	unsigned char ucResponse[267];		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	unsigned short usChecksum = 0;		//	calculated checksum for the downloaded page
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 5, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to download page %i on node %i\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nMSB Page: %#04x\tLSB Page: %#04x",
		usPage, usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, ucResponse, 267, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	calculate the checksum	*/
	for (int i = 1; i < 265; i++)
		usChecksum += ucResponse[i];
	if ((usChecksum & 0xFFFF) != (ucResponse[265] << 8) + ucResponse[266]) {
		/*	append to the command description	*/
		strFormat.Format("A bad checksum was returned for downloading page %i on node %i\r\nReturned Checksum: %#04x\r\nCalculated Checksum: %#04x", 
			usPage, usNode, (ucResponse[265] << 8) + ucResponse[266], usChecksum & 0xFFFF);
		/*	the return will take up multiple lines so put the command information in for every line	*/
		
		int iSize = static_cast<int>(strCmd_info.GetCount());
		for (int i = iSize; i <= iPos; i++) {
			crCmd_color[i] = COLOR_RECIEVE;
			strCmd_info[i] = strFormat;
		}
		return ERROR_BAD_CHECKSUM;
	}

	/*	passed checksum so store the result in the usResponse array	*/
	for (int i = 0; i < 132; i++)
		usResponse[i] = (ucResponse[1 + i * 2] << 8) + ucResponse[2 + i * 2];

	/*	append to the command description	*/
	strFormat.Format("Successful page download attempt for page %i on node %i\r\nChecksum: %#04x", 
		usPage, usNode, (ucResponse[265] << 8) + ucResponse[266]);
	/*	the return will take up multiple lines so put the command information in for every line	*/
	int iSize = static_cast<int>(strCmd_info.GetCount());
	for (int i = iSize; i <= iPos; i++)
		strCmd_info[i] = strFormat;

	return 0;
}

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
				/* in */ unsigned char ucID)
{
	unsigned char ucCmd[4] =	{	0x0C,			//	trigger id command byte
									usNode >> 8,	//	MSB of the node's address
									usNode & 0xFF,	//	LSB of the node's address
									ucID			//	ID of the node
								};
	unsigned char ucResponse;		//	the value returned from the trigger
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 4, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to start a trigger with ID %i on node %i.\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nTrigger ID: %#04x",
		ucID, usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	check for the correct return value	*/
	if (ucResponse != 0x0C) {
		/*	append to the command description	*/
		strFormat.Format("Failed with a bad response to start a trigger with ID %i on node %i\r\nResponse: %#04x", ucID, usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Successful attempt to trigger with ID %i on node %i.", ucID, usNode);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
			 /* in */ unsigned short usNode)
{
	unsigned char ucCmd[3] =	{	0x32,			//	sleep command byte
									usNode >> 8,	//	MSB of the node's address
									usNode & 0xFF	//	LSB of the node's address
								};
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 3, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to sleep node %i.\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x",
		usNode, ucCmd[0], ucCmd[1], ucCmd[2]);
	strCmd_info[iPos] = strFormat;

	/*	there is no response from the serial port	*/

	return 0;
}

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
				/* in */ unsigned short usNode)
{
	unsigned char ucCmd[7] =	{	0x06,			//	erase command byte
									usNode >> 8,	//	MSB of the node's address
									usNode & 0xFF,	//	LSB of the node's address
									0x08,			//	command byte
									0x10,			//	command byte
									0x0C,			//	command byte
									0xFF			//	command byte
								};
	unsigned char ucResponse;		//	the value returned from the trigger
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 7, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to erase node %i.\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Bytes: %#04x %#04x %#04x %#04x",
		usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	check for the correct return value	*/
	if (ucResponse != 0x06) {
		/*	append to the command description	*/
		strFormat.Format("Failed attempt to erase node %i with an incorrect response.\r\nResponse: %#04x", usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Successful attempt to erase node %i.", usNode);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
					/* in */ unsigned short usNode)
{
	unsigned char ucCmd[3] =	{	0x38,			//	stream command byte
									usNode >> 8,	//	MSB of the node's address
									usNode & 0xFF,	//	LSB of the node's address
								};
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 3, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to stream node %i.\r\nCommand Byte: %#04x\tMSB Node Address: %#04x\tLSB Node Address: %#04x",
		usNode, ucCmd[0], ucCmd[1], ucCmd[2]);
	strCmd_info[iPos] = strFormat;

	/*	align the stream to the first sweep by dumping all incoming garbage and
	*	finding the very first 0xFF	*/
	unsigned char ucGarbage = 0, ucPrev_garbage = 0x01;
	unsigned int uiAA_counter = 0;
	DWORD dwResult_len = 0;
	do
	{
		if ((iResult = port_read(hPort, &ucGarbage, 1, dwBytes_read, iPos)) != 0) {	//	do error correction on this
			return -1;
		};	
		if (ucPrev_garbage != 0xAA)	uiAA_counter = 0;
		else	ucPrev_garbage = ucGarbage;

		if (ucGarbage == 0xAA) {
			if (++uiAA_counter > 4) 		// 5 continuous 0xAA's is considered the end of a stream
				return ERROR_END_OF_STREAM;	// end of stream flags encountered
			
			strFormat.Format("Possible end of stream flag.\r\nByte: %#04x\r\n", ucGarbage);
			strCmd_info[iPos] = strFormat;
		}
		else if (ucGarbage == 0xFF) {	// 0xFF is the flag for the beginning of a sweep
			strFormat.Format("Initial sweep flag byte.\r\nByte: %#04x\r\n", ucGarbage);
			strCmd_info[iPos] = strFormat;
		}
		else {
			strFormat.Format("Garbage byte from stream initialization.\r\nByte: %#04x\r\n", ucGarbage);
			strCmd_info[iPos] = strFormat;
		}
	} while (ucGarbage != 0xFF);
	return 0;
}

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
					/* in */ unsigned char ucNum_channels)
{
	dynamic_buffer<unsigned char> ucResponse(ucNum_channels * 2 + 2);	//	2 bytes for each channel, 1 for the checksum, 1 for next 0xFF flag
	DWORD dwBytes_read = 0;			//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat,				//	formatted string for output to command information window
			strFormat_temp;			//	temporary string for assistance with formatting output text
	/*	attempt to read 1 sweep	*/
	if ((iResult = port_read(hPort, ucResponse, ucNum_channels * 2 + 2, dwBytes_read, iPos)) != 0) {
		/*	check for the end of stream responses	*/
		bool isEnd = true;
		for (DWORD i = 0; i < dwBytes_read; i++)
			if (ucResponse[i] != 0xAA)
				isEnd = false;
		if (isEnd) {
			strFormat = "End of stream flags.";
			strCmd_info[iPos] = strFormat;
			return ERROR_END_OF_STREAM;
		}
		else {
			strFormat = "Unknown packet format.";
			strCmd_info[iPos] = strFormat;
		}
		return ERROR_MISALIGNED_SWEEP_PACKET;	//	add errors and check for 0xAA's
	}

	/*	This long block of code is for adding command information to the console and
	*	has nothing to do with the 2400 Mhz protocol.	*/
	strFormat.Format("Successful sweep recieved.\r\n");
	for (int i = 0; i < ucNum_channels; i++) {
		strFormat_temp.Format("MSB Channel: %#04x\tLSB Channel: %#04x\tCalculated Value: %i\r\n",
			ucResponse[i * 2], ucResponse[i * 2 + 1], ((ucResponse[i * 2] << 8) + ucResponse[i * 2 + 1]) >> 1);
		strFormat += strFormat_temp;
	}
	strFormat_temp.Format("Checksum: %#04x\t\t", ucResponse[ucNum_channels * 2]);
	strFormat += strFormat_temp;
	if (ucResponse[ucNum_channels * 2 + 1] == 0xFF)
		strFormat_temp.Format("Sweep Flag: %#04x\r\n", ucResponse[ucNum_channels * 2 + 1]);
	else if (ucResponse[ucNum_channels * 2 + 1] == 0xAA)
		strFormat_temp.Format("End of Stream Flag: %#04x\r\n", ucResponse[ucNum_channels * 2 + 1]);
	else
		strFormat_temp.Format("Unknown Trailing Byte: %#04x\r\n", ucResponse[ucNum_channels * 2 + 1]);
	strFormat += strFormat_temp;
	strCmd_info[iPos] = strFormat;

	/*	calculate the checksum to make sure it's right	*/
	unsigned char ucChecksum = 0;
	for (int i = 0; i < ucNum_channels * 2; i++)
		ucChecksum += ucResponse[i];

	/*	check to make sure calculated and returned checksums match	*/
	if (ucChecksum != ucResponse[ucNum_channels * 2])
		return ERROR_BAD_CHECKSUM;	//	bad checksum

	/*	put in the legitimate data	*/
	for (i = 0; i < ucNum_channels; i++)
		pusResponse[i] = (ucResponse[i * 2] << 8) + ucResponse[i * 2 + 1];
	return 0;
}

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
*		Length:		1 Byte
*		Byte 1:		0xAA		Valid packet response from base station
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			usNode is an unsigned short holding the node's address to ping
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int nd_low_duty_cycle_init(/* in */ int handle,
						   /* in */ unsigned short usNode)
{

	unsigned char ucCmd[10] = {	0xAA,			//	SOP 
								0x05,			//	Message Type
								0x00,			//	Address Mode
								usNode >> 8,	//	MSB of Node Address
								usNode & 0xFF,	//	LSB of Node Address
								0x02,			//	Command Length
								0x00,			//	MSB of Command Byte
								0x38,			//	LSB of Command Byte
								(0x3F + (usNode >> 8) + (usNode & 0xFF)) >> 8,		//	MSB of the checksum
								(0x3F + (usNode >> 8) + (usNode & 0xFF)) & 0xFF		//	LSB of the checksum
	};

	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 10, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to start low duty cycle on node %i\r\nSOP: %#04x\t\t\tMessage Type: %#04x\t\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tMSB of Command Byte: %#04x\tLSB of Command Byte: %#04x\r\nMSB of Checksum: %#04x\t\tLSB of Checksum: %#04x",
			usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6], ucCmd[7],
			ucCmd[8], ucCmd[9]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	the only legitimate response is a 0xAA	*/
	if (ucResponse != 0xAA) {
		/*	append to the command description	*/
		strFormat.Format("Failed to start low duty cycle on node %i\r\nResponse: %#04x", usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Low duty cycle attempt successful for node %i\r\nResponse: %#04x", usNode, ucResponse);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
							 /*	in	*/ unsigned char ucNumChannels)
{
	DWORD dwPacketLen = ucNumChannels * 2 + 16;	            //	total length of the packet
	dynamic_buffer<unsigned char> ucResponse(dwPacketLen);	//	holds the low duty cycle packet
	DWORD dwBytes_read = 0;			//	number of bytes read from the port
	int iResult,						//	return value of port read/write functions
		iPos;							//	position in the console that the command was written to
	CString strFormat;					//	formatted string for output to command information window

	if ((iResult = port_read(hPort, ucResponse, dwPacketLen, dwBytes_read, iPos)) != 0) {
		return iResult;
	}

	strFormat.Format("Low duty cycle packet received.\r\nSOP: %#04x\t\t\tMessage Type: %#04x\t\t\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tApplication Identifier: %#04x\t\tChannel Mask: %#04x\r\nSample Rate: %#04x\t\tData Type: %#04x\r\nMSB Timer Tick: %#04x\t\tLSB Timer Tick: %#04x\r\n", 
		ucResponse[0], ucResponse[1], ucResponse[2], ucResponse[3], ucResponse[4], ucResponse[5], ucResponse[6], ucResponse[7],
		ucResponse[8], ucResponse[9], ucResponse[10], ucResponse[11]);
	CString strTemp;
	/*	Even though we already know the number of channels, use the channel mask to decide which data goes to
	*	which channel.	*/
	int iResponseLoc = 12;	//	position in the ucResponse array for channel data
	for (int iCurChan = 0; iCurChan < 8; iCurChan++)
		if ((ucResponse[7] >> iCurChan) & 1) {
			strTemp.Format("MSB Channel %i Data: %#04x\tLSB Channel %i Data: %#04x\t\tCalculated Value: %i\r\n", iCurChan + 1, ucResponse[iResponseLoc], iCurChan + 1, ucResponse[iResponseLoc + 1], ((ucResponse[iResponseLoc] << 8) + ucResponse[iResponseLoc + 1]) >> 1);
			iResponseLoc += 2;
			strFormat += strTemp;
		}
	
	strTemp.Format("LQI: %#04x\t\t\tRSSI: %#04x\t\tMSB Checksum: %#04x\tLSB Checksum: %#04x", ucResponse[iResponseLoc], ucResponse[iResponseLoc + 1], ucResponse[iResponseLoc + 2], ucResponse[iResponseLoc + 3]);
	strFormat += strTemp;
	
	int iSize = static_cast<int>(strCmd_info.GetSize());
	do {
		strCmd_info[iSize] = strFormat;
	} while (iSize++ < iPos);

	return 0;
}

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
						   /* in */ unsigned short usNode)
{
	unsigned char ucCmd[10] = {	0xAA,			//	SOP 
								0x05,			//	Message Type
								0x00,			//	Address Mode
								usNode >> 8,	//	MSB of Node Address
								usNode & 0xFF,	//	LSB of Node Address
								0x02,			//	Command Length
								0x00,			//	MSB of Command Byte
								0x39,			//	LSB of Command Byte
								(0x40 + (usNode >> 8) + (usNode & 0xFF)) >> 8,		//	MSB of the checksum
								(0x40 + (usNode >> 8) + (usNode & 0xFF)) & 0xFF		//	LSB of the checksum
	};

	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 10, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to start high speed streaming on node %i\r\nSOP: %#04x\t\t\tMessage Type: %#04x\t\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tMSB of Command Byte: %#04x\tLSB of Command Byte: %#04x\r\nMSB of Checksum: %#04x\t\tLSB of Checksum: %#04x",
			usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6], ucCmd[7],
			ucCmd[8], ucCmd[9]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	the only legitimate response is an 0xAA	*/
	if (ucResponse != 0xAA) {
		/*	append to the command description	*/
		strFormat.Format("Failed to start high speed stream on node %i\r\nResponse: %#04x", usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("High speed stream attempt successful for node %i\r\nResponse: %#04x", usNode, ucResponse);
	strCmd_info[iPos] = strFormat;

	return 0;
}


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
int nd_high_speed_stream_packet(/* in */ int handle)
{
	DWORD dwBytes_read = 0;			//	number of bytes read from the port
	int iResult,						//	return value of port read/write functions
		iPos;							//	position in the console that the command was written to
	CString strFormat;					//	formatted string for output to command information window

	unsigned char ucPacketHeader[14] = {0};

	/* Read the header of the packet to detrmine payload size */
	if ((iResult = port_read(hPort, ucPacketHeader, 14, dwBytes_read, iPos)) != 0) {
		return iResult;
	}

	strFormat.Format("High speed stream packet received.\r\n"
	                 "SOP: %#04x\t\tMessage Type: %#04x\tAddress Mode: %#04x\r\n"
	                 "MSB Node Address: %#04x\tLSB Node Address: %#04x\tCommand Length: %#04x\r\n"
					 "Application Identifier: %#04x\tChannel Mask: %#04x\tStream Rate: %#04x\r\n"
					 "Sample Count: %#04x\tTimer Value: %#04x %#04x %#04x %#04x\r\n",
		ucPacketHeader[0], ucPacketHeader[1],  ucPacketHeader[2],
		ucPacketHeader[3], ucPacketHeader[4],  ucPacketHeader[5],
		ucPacketHeader[6], ucPacketHeader[7],  ucPacketHeader[8],
		ucPacketHeader[9], ucPacketHeader[10], ucPacketHeader[11], ucPacketHeader[12],  ucPacketHeader[13]);
	CString strTemp;

	unsigned char ucDataLength  = ucPacketHeader[5];
	unsigned char ucSampleCount = ucPacketHeader[9];
	unsigned char ucChannelMask = ucPacketHeader[7];

	/* Part of the packets data was read as part of the header */
	ucDataLength -= 8; // 8=>Application Identifier(1) + Channel Mask(1) + Stream Rate(1) + Sample Count(1) + Timer Value(4)

	/* also read RSSI, LQI and the checksum wich are not included in the packet data length */
	ucDataLength += 4; // 4 => RSSI(1 byte) + LQI(1 byte) + Checksum(2 bytes)
	
	/* read the rest of the Packet */
	dynamic_buffer<BYTE> responseData(ucDataLength);
	if ((iResult = port_read(hPort, responseData, ucDataLength, dwBytes_read, iPos)) != 0) {
		return iResult;
	}

	/* create a header for the detailed display of the data */
	CString strHeaderLine1 = "Sample \t";
	CString strHeaderLine2 = "       \t";
	for (int iCurChan = 0; iCurChan < 8; iCurChan++)
	{
		if ((ucChannelMask >> iCurChan) & 1)
		{
			strHeaderLine1.AppendFormat("     Channel %d\t\t", iCurChan+1);
			strHeaderLine2.Append("MSB   LSB   (Value)\t\t");
		}
	}

	/* add the header to the description */
	strFormat += strHeaderLine1;
	strFormat += "\r\n";
	strFormat += strHeaderLine2;
	strFormat += "\r\n";

	/* count the channels */
	int iChannelCount = 0;
	for (int iCurChan = 0; iCurChan < 8; iCurChan++)
	{
		if ((ucChannelMask >> iCurChan) & 1)
		{
			iChannelCount++;
		}
	}

	/* proccess each sample, placing the data in the description text */
	int iResponseLoc = 0;
	for(int iSample = 0; iSample < ucSampleCount; iSample++)
	{
		strTemp.Format("%d\t", iSample+1);

		//	Use the channel mask to decide which data goes to which channel.
		int channelOffset = 0;
		for (int iCurChan = 0; iCurChan < 8; iCurChan++)
		{
			if ((ucChannelMask >> iCurChan) & 1)
			{
				iResponseLoc = (iSample*iChannelCount + channelOffset)*2;
				int iValue = ((responseData[iResponseLoc] << 8) + responseData[iResponseLoc+1]);
				strTemp.AppendFormat("%#04x  %#04x  (%#05d)     \t", responseData[iResponseLoc], responseData[iResponseLoc+1], (responseData[iResponseLoc] << 8) + responseData[iResponseLoc+1]);
				channelOffset++;
			}
		}

		strFormat += strTemp;
		strFormat += "\r\n";
	}

	//the last 4 bytes are the RSSI, LQI, and checksum
	strTemp.Format("LQI: %#04x\tRSSI: %#04x\r\n", responseData[ucDataLength - 4], responseData[ucDataLength - 3]);
	strFormat += strTemp;
	
	WORD wReturedChecksum = (responseData[ucDataLength - 2] << 8 ) + responseData[ucDataLength -1];

	//calculate the checksum
	WORD wCalculatedChecksum = 0;
	for(int i = 1; i < 14; i++)
	{
		wCalculatedChecksum += ucPacketHeader[i];
	}

	for(int i = 0; i < ucDataLength-4; i++)
	{
		wCalculatedChecksum += responseData[i];
	}

	//out put the returned and calculated checksums, they should always be identical
	ASSERT(wReturedChecksum == wCalculatedChecksum);
	strFormat.AppendFormat("Returned Checksum:\t %#04x\r\n", wReturedChecksum);
	strFormat.AppendFormat("Calculated Checksum:\t %#04x\r\n", wCalculatedChecksum);

	//add the detailed data to each line of the raw output
	int iSize = static_cast<int>(strCmd_info.GetSize());
	do {
		strCmd_info[iSize] = strFormat;
	} while (iSize++ < iPos);

	return 0;
}


/***********************
*	nd_discovery_packet
*	nd_discovery_packet attempts to receive a node discovery packet from any node that sends one over the port, hPort.
*	Good Response:
*		Length:		1 Byte
*		Byte 1:		0xAA		SOP
*		Byte 2:		0x00		Message Type				OR		(Byte 2 & 0x08) == 0x00
*		Byte 3:		0x00		Address Mode				OR		Byte 3 == 0x07
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
int nd_discovery_packet(/* in */ int handle, /* in */ int iTimeout)
{
	unsigned char	ucResponse[13],		//	buffer to store the discovery packet in
					ucLength = 13;		//	the length of the discovery packet
	DWORD dwBytes_read;
	int iResult,						//	return value of port read/write functions
		iPos;							//	position in the console that the command was written to
	CString strFormat;					//	formatted string for output to command information window

	if ((iResult = port_read(hPort, ucResponse, ucLength, dwBytes_read, iPos)) != 0) {
		return iResult;
	}

	/*	Perform a quick verification of the packet.  This verification only verifies
		the SOP, Message type, Address mode, and packet length.  For a more thorough verification,
		also verify that the radio channels and model numbers are valid.	*/
	if (!(dwBytes_read == 13 && ucResponse[0] == 0xAA && 
			((ucResponse[1] == 0x00 && ucResponse[2] == 0x00) || ((ucResponse[1] & 0x08) == 0x00 && ucResponse[2] == 0x07)) &&
			ucResponse[5] == 0x03)) {
		strFormat.Format("Received data was invalid.\r\n");
		CString strTemp;
		for (DWORD i = 0; i < dwBytes_read; i++) {
			strTemp.Format("Byte %i: %#04x\t\t", i + 1, ucResponse[i]);
			if (i % 3 == 1 && i != 0)
				strTemp += "\r\n";
			strFormat += strTemp;
		}
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	Translate the packet and output the description to the list of packet descriptions.	*/
	strFormat.Format("Node Discovery Packet received.\r\nSOP: %#04x\t\t\tMessage Type: %#04x\t\t\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tRadio Channel: %#04x\r\nMSB Model Number: %#04x\tLSB Model Number: %#04x\r\nLQI: %#04x\t\t\tRSSI: %#04x\r\nMSB Checksum: %#04x\t\tLSB Checksum: %#04x", 
		ucResponse[0], ucResponse[1], ucResponse[2], ucResponse[3], ucResponse[4], ucResponse[5], ucResponse[6], ucResponse[7],
		ucResponse[8], ucResponse[9], ucResponse[10], ucResponse[11], ucResponse[12]);
	strCmd_info[iPos] = strFormat;

	return 0;
}




/***********************
*	nd_kill_node
*	nd_kill_node attempts to stop a node that is in a state that doesn't accept commands. ie streaming, LDC etc
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
*		Byte 1:		0xAA		Valid packet response from base station
*
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
					  /* in */ unsigned short usNode)
{

	
	

	unsigned char ucCmd[10] = {	0xAA,			//	SOP 
								0xFE,			//	Message Type
								0x00,			//	Address Mode
								usNode >> 8,	//	MSB of Node Address
								usNode & 0xFF,	//	LSB of Node Address
								0x02,			//	Command Length
								0x00,			//	MSB of Command Byte
								0x90,			//	LSB of Command Byte
								(0x190 + (usNode >> 8) + (usNode & 0xFF)) >> 8,		//	MSB of the checksum
								(0x190 + (usNode >> 8) + (usNode & 0xFF)) & 0xFF	//	LSB of the checksum
	};

	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	// before we send the kill node command send a byte to interupt the base station
	// this will stop it from listening for streaming, HSS or LDC data
	unsigned char ucByte = 0x01;
	if ((iResult = port_write(hPort, &ucByte, 1, dwBytes_written, iPos)) != 0)
		return iResult;
	strCmd_info[iPos] = "Send byte to stop the base station from streaming.";

	//now purge the buffer
	port_purge(hPort); 

	/*	send the command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 10, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to kill node on node %i\r\nSOP: %#04x\t\t\tMessage Type: %#04x\t\tAddress Mode: %#04x\r\nMSB Node Address: %#04x\tLSB Node Address: %#04x\r\nCommand Length: %#04x\t\tMSB of Command Byte: %#04x\tLSB of Command Byte: %#04x\r\nMSB of Checksum: %#04x\t\tLSB of Checksum: %#04x",
			usNode, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3], ucCmd[4], ucCmd[5], ucCmd[6], ucCmd[7],
			ucCmd[8], ucCmd[9]);
	strCmd_info[iPos] = strFormat;


	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	the only legitimate response is a 0xAA	*/
	if (ucResponse != 0xAA) {
		/*	append to the command description	*/
		strFormat.Format("Failed to start kill node on node %i\r\nResponse: %#04x", usNode, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Kill node command started successful for node %i\r\nResponse: %#04x", usNode, ucResponse);
	strCmd_info[iPos] = strFormat;

	return 0;
}

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
					    /* out */ bool& bNodeKilled)
{
	bNodeKilled = false;

	unsigned char ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/* set the read timeout to 100 ms */

	/*	read the response from the serial port	*/
	iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos);
	if(iResult== ERROR_MISMATCHING_BYTE_LENGTH){iResult = 0;}//no response is a valid state since we are polling, waiting for the return
	if(iResult!= 0)
		return iResult;

	/*	did we get any data	*/
	if(dwBytes_read > 0)
	{
        /*	node successfully stopped */
		if (ucResponse == 0x90)
		{
			/*	append to the command description	*/
			strFormat.Format("Kill node successful\r\nResponse: %#04x", ucResponse);
			strCmd_info[iPos] = strFormat;	

			bNodeKilled = true;
		}

		/*	kill node canceld by the usernode successfully stopped */
		else if(ucResponse == 0x21)
		{
			/*	append to the command description	*/
			strFormat.Format("Kill node aborted\r\nResponse: %#04x", ucResponse);
			strCmd_info[iPos] = strFormat;
			return ERROR_KILL_NODE_ABORTED;
		}

		/*	invalid response */
		else
		{
			/*	append to the command description	*/
			strFormat.Format("Failed to kill node\r\nResponse: %#04x", ucResponse);
			strCmd_info[iPos] = strFormat;
			return ERROR_INVALID_RESPONSE;
		}
	}

	return 0;
}

/***********************
*	nd_kill_node_abort
*	nd_kill_node_abort sneds a byte to the basestation so the basestation will stop trtying to kill the node. 
*	Command:
*		Length:		1 Byte
*		Byte 1:		0x00		
*   
*   Note:   nd_kill_node_status should be called after aborting to get the status
*
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			nd_kill_node_init was previously called
*
***********************/
int nd_kill_node_abort(/* in */ int handle)
{
	
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	unsigned char byteCommand = 0x0;

	/*	send a byte to the base station to stop the kill command */
	if ((iResult = port_write(hPort, &byteCommand, 1, dwBytes_written, iPos)) != 0)
		return iResult;

	strCmd_info[iPos] = "sent byte to abort \"Kill Node\" command";

	return iResult;

}
