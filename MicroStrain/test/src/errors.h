/****************************
*	errors.h
*	errors.h holds error code values for any errors that 
*	may occur during use of the 2400 Mhz demo application.
****************************/

#ifndef	ERRORS_H
#define	ERRORS_H

/*	number of bytes written to the port != number of bytes to write	*/
#define	ERROR_MISMATCHING_BYTE_LENGTH	0x0001	//	expected byte length != actual byte length
#define	ERROR_INVALID_RESPONSE			0x0002	//	invalid response on serial port
#define	ERROR_BAD_CHECKSUM				0x0003	//	real checksum != calculated checksum
#define	ERROR_END_OF_STREAM				0x0004	//	end of stream encountered
#define	ERROR_MISALIGNED_SWEEP_PACKET	0x0005	//	mis-aligned/unrecognized sweep packet
#define	ERROR_KILL_NODE_ABORTED	        0x0006	//	the kill node command was unsuccessful and aborted by the user
#endif