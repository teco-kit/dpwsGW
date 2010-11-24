/*********************
*	base_station.h
*	base_station.h holds the declarations for each individual
*	function that applies to the base station.  For a better understanding
*	of each function, please, see the implementation file, base_station.cpp
*	and reference the desired function.
*********************/

#ifndef	BASE_STATION_H
#define	BASE_STATION_H

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
int bs_ping(/* in */ int handle);

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
int bs_eeprom_read(/* in */ int handle,
				   /* in */ unsigned char ucLocation,
				   /* out */ unsigned short & usResponse);

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
int bs_eeprom_write(/* in */ int handle,
					/* in */ unsigned char ucLocation,
					/* in */ unsigned short usValue);

#endif
