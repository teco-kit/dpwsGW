/*********************
*	Config holds the constant value definitions for all
*	of the 2400 Mhz SDK demonstration program.  Descriptions
*	on how each value affects the program are written with that
*	value.
*********************/

#ifndef	CONFIG_H
#define	CONFIG_H

/*	The following values define the colors for different types of messages
*	written to the console window	*/
#define	COLOR_NEUTRAL	RGB(0, 0, 0)		//	color for all neutral messages
#define	COLOR_SEND		RGB(255, 0, 0)		//	color for all sent messages
#define	COLOR_RECIEVE	RGB(0, 0, 255)		//	color for all recieved messages
#define	COLOR_OUTLINE	RGB(128, 128, 128)	//	color to outline selected item with

/*	The following two values set range for the lowest and highest
*	values a user may enter for the port number on the dialog box, respectively.	*/
#define	PORT_NUM_LO		1
#define	PORT_NUM_HI		256

/*	The following two values are the ranges for the low and high ends
*	when entering an EEPROM location for the base station and node in the
*	application's dialog box.	*/
#define	EEPROM_LOCATION_LO	0
#define	EEPROM_LOCATION_HI	256

/*	The following two values are the low and high end values of what can be written
*	to an EEPROM address	*/
#define	EEPROM_VALUE_LO		0
#define	EEPROM_VALUE_HI		65535

/*	The following two values are the low and high end values of what can be used
*	as a node address	*/
#define	NODE_ADDRESS_LO	1
#define	NODE_ADDRESS_HI	65535

/*	The following two values are the low and high end values of what can be used
*	as a page to download from the node	*/
#define	NODE_PAGE_LO	0
#define	NODE_PAGE_HI	8191

/*	The following two values are the low and high end values of what can be used
*	as an id value for a trigger	*/
#define	TRIGGER_ID_LO	0
#define	TRIGGER_ID_HI	255

/*	The following two values are the low and high end values of waht can be used
*	for the number of sweeps during a stream	*/
#define	SWEEPS_LO		100
#define	SWEEPS_HI		65500

/*	This value is used for the stream setup.  If a user sets the number of sweeps
*	beyond this value, they may have a ridiculous amount of output to the screen.
*	If the user sets the number of sweeps beyond this threshold, the application
*	will warn them.	*/
#define	SWEEP_WARN		300

/*	The following two values are the enable and disable values used
*	for the continuous streaming flag */
#define	CONTINUOUS_ENABLED		1
#define	CONTINUOUS_DISABLED   	0

/*	EEPROM locations	*/
#define	EE_CHANNEL_MASK				12	//	active channel mask
#define	EE_SAMPLES_PER_DATA_SET		16	//	number of sweeps
#define EE_CONTINUOUS_STREAMING    100  //  enable or disable continuous streaming

#endif