/*********************
*	base_station.cpp
*	base_station.cpp holds the implementation for each individual
*	function that applies to the base station.  For a better understanding
*	of each function, please, see reference the desired function.
*********************/

#include "2400_sdk.h"
#include "stdafx.h"

/*	base station functionality	*/
#include "base_station.h"

/*	used for serial (comm) port functionality	*/
#include "serial.h"

/*	used for console output functionality	*/
#include "console_commands.h"
extern CMapPosToCString strCmd_info;

/***********************
*	bs_ping
*	bs_ping attempts to ping the base station attached to the given
*	port handle, hPort.
*	Command Format
*		Length:		1 byte
*		Byte 1:		0x01
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x01
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int bs_ping(/* in */ HANDLE hPort)
{
	TRACE("bs_ping()\n");
	unsigned char	ucCmd = 0x01,	//	command to send
					ucResponse;		//	holds the response
	DWORD	dwBytes_written = 0,	//	number of bytes written to the port
			dwBytes_read = 0;		//	number of bytes read from the port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the 0x01 command to the serial port	*/
	if ((iResult = port_write(hPort, &ucCmd, 1, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to ping base station on port handle %i.\r\nCommand Byte: %#04x",
		hPort, ucCmd);
	strCmd_info[iPos] = strFormat;

	/*	attempt to read the correct response, 0x01, from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	if (ucResponse != 0x01) {
		/*	append to the command description	*/
		strFormat.Format("Attempt to ping base station on port handle %i.", hPort);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	strFormat.Format("Attempt to ping base station on port handle %i.", hPort);
	strCmd_info[iPos] = strFormat;
	return 0;
}

/***********************
*	bs_eeprom_read
*	bs_eeprom_read attempts to read an eeprom location, ucLocation, from
*	the base station opened by the handle, hPort, and store it in usResponse
*	Command Format
*		Length:		2 bytes
*		Byte 1:		0x72
*		Byte 2:		Location to read
*	Good Response:
*		Length:		3 bytes
*		Byte 1:		0x72
*		Byte 2:		MSB of EEPROM Location
*		Byte 3:		LSB of EEPROM Location
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			ucLocation is an unsigned char holding the location to read
*			usResponse is an unsigned short that will hold the read value
*	POST:	if successful, 0 is returned and usResponse holds the value read from
*				the EEPROM
*			otherwise, some error code
***********************/
int bs_eeprom_read(/* in */ HANDLE hPort, 
				   /* in */ unsigned char ucLocation,
				   /* out */ unsigned short & usResponse)
{
	TRACE("bs_eeprom_read()\n");

	unsigned char ucCmd[2] =	{	0x72,	//	read command byte
									ucLocation	//	location to read
								};
	unsigned char ucResponse[3];	//	will hold the 2 byte response from the base station
	DWORD	dwBytes_written,	//	number of bytes written to the serial port
			dwBytes_read;		//	number of bytes read from the serial port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the read command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 2, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to read EEPROM %i from base station on port handle %i.\r\nCommand Byte: %#04x\tEEPROM Location: %#04x",
		ucLocation, hPort, ucCmd[0], ucCmd[1]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, ucResponse, 3, dwBytes_read, iPos)) != 0)
		return iResult;
	
	/*	check to make sure the response is a 0x72	*/
	if (ucResponse[0] != 0x72) {
		/*	append to the command description	*/
		strFormat.Format("Failed attempt to read EEPROM %i from base station on port handle %i.\r\nResponse: %#04x", hPort, ucResponse[0]);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	combine the 2 byte response into an unsigned short and store it	*/
	usResponse = (ucResponse[1] << 8) + ucResponse[2];

	/*	append to the command description	*/
	strFormat.Format("Successful attempt to read EEPROM %i from base station on port handle %i.\r\nResult: %i", ucLocation, hPort, usResponse);
	strCmd_info[iPos] = strFormat;

	return 0;
}

/***********************
*	bs_eeprom_write
*	bs_eeprom_write attempts to write to an eeprom location, ucLocation, a value,
*	usValue, to the base station opened by port, hPort.
*	Command Format
*		Length:		4 bytes
*		Byte 1:		0x77
*		Byte 2:		Location to read
*		Byte 3:		MSB of EEPROM value to write
*		Byte 4:		LSB of EEPROM value to write
*	Good Response:
*		Length:		1 byte
*		Byte 1:		0x77
*	PRE:	hPort is a legitimate, open, port handle of type HANDLE
*			ucLocation is an unsigned char holding the location to read
*			usValue is an unsigned short that will be written to the EEPROM
*	POST:	if successful, 0 is returned
*			otherwise, some error code
***********************/
int bs_eeprom_write(/* in */ HANDLE hPort,
					/* in */ unsigned char ucLocation,
					/* in */ unsigned short usValue)
{
	TRACE("bs_eeprom_write()\n");
	unsigned char ucCmd[4] = {	0x77,				//	eeprom write command byte
								ucLocation,			//	eeprom location
								usValue >> 8,		//	MSB of value to write to eeprom
								usValue & 0xFF		//	LSB of value to write to eeprom
							};
	unsigned char ucResponse;			//	the return value of the write command
	DWORD	dwBytes_written,	//	number of bytes written to the serial port
			dwBytes_read;		//	number of bytes read from the serial port
	int iResult,					//	return value of port read/write functions
		iPos;						//	position in the console that the command was written to
	CString strFormat;				//	formatted string for output to command information window

	/*	send the read command to the serial port	*/
	if ((iResult = port_write(hPort, ucCmd, 4, dwBytes_written, iPos)) != 0)
		return iResult;

	/*	append to the command description	*/
	strFormat.Format("Attempt to write value %i to EEPROM %i onto base station with port handle %i.\r\nCommand Byte: %#04x\tEEPROM Location: %#04x\r\nMSB Write Value: %#04x\tLSB Write Value: %#04x",
		usValue, ucLocation, hPort, ucCmd[0], ucCmd[1], ucCmd[2], ucCmd[3]);
	strCmd_info[iPos] = strFormat;

	/*	read the response from the serial port	*/
	if ((iResult = port_read(hPort, &ucResponse, 1, dwBytes_read, iPos)) != 0)
		return iResult;

	/*	check to make sure the response is the legitimate write response	*/
	if (ucResponse != 0x77) {
		/*	append to the command description	*/
		strFormat.Format("Failed attempt to write value %i to EEPROM %i onto base station with port handle %i with bad response.\r\nResponse: %#04x", usValue, ucLocation, hPort, ucResponse);
		strCmd_info[iPos] = strFormat;
		return ERROR_INVALID_RESPONSE;
	}

	/*	append to the command description	*/
	strFormat.Format("Successful attempt to write value %i to EEPROM %i onto base station with port handle %i.", usValue, ucLocation, hPort);
	strCmd_info[iPos] = strFormat;

	return 0;
}
