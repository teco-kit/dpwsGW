 
// APPLICATION FILE
// uses ACK, RTC & PRS.
//
#define this(F) F

#case

#opt 6

#define byte	BYTE
#define boolean BOOLEAN
#define true	TRUE
#define false	FALSE
#define global	GLOBAL

#define INC(X) #X
#define ACLAnyDataIsNew ACLDataIsNew

#include "system/acltypes.h"	// the acl types

#include "boards/18f6720.h"
#include "boards/18f6720_R.h"

#include INC(boards/BOARD.h)

#include INC(boards/SENSORBOARD.h)

#include "boards/wdt18fxx20.h"
#include "boards/18f6720.c"

// We use V9 to be more robust !?

#include "boards/wdt18fxx20.c"

#if BOARD_ID_LOW > 229
#include "boards/ballswitch.c"
#include "boards/ds2431.c"		//id chip
#include "boards/AT45DB041.c"	//flash
#include "boards/owmb.c"		//speaker
#else
#include "boards/pci2c.c"
#include "boards/pceeprom.c"	//flash
#endif

#include INC(boards/BOARD.c)


#include "system/awarecon.h"			
#include "system/awarecon.c"	


//#include INC(boards/SENSORBOARD.c)
#include <boards/SSimp202.c>

#ifndef DUMMY_MODE
#include "boards/SensorI2C.c"		
//+++++ include the sensors your want to use ++++++++++++++++++++++++++++++++++
//#include "piezo.c"
//#include "sensors/tsl250.c"
//#include "sensors\tc74.c"
//#include "sensors/fsr.c"
#include "sensors/SP0101NC1.c"
#include "sensors/adxl210.c"
#include "sensors/adxl210_z.c"
#include "sensors/mcp9800.c"
#include "sensors/tsl2550.c"
#else
#endif

#include "sensors/voltage.c"

#ifndef BOARD_ID_LOW
#error NO BOARD DEFINED
#endif

#fuses hs,noprotect,nobrownout,nolvp,put,nowdt


#ifdef TEST
#include "prs_test.c" 								// programmable sensors
#else#include "prs74.c" 								// programmable sensors
#endif



// this function is called from the fsm at the end of an rf slot
// make sure that it terminates in time
#separate
void SlotEndCallBack()
{
#ifndef TEST
	PRSRun();
#endif
}


void main()
{
#ifdef TEST
 PRSTest1();
#else
	//TYPICAL STARTUP FLOW
	//*****************************************************************
	PCInit();											// is not dangerous, because all pins are set correct . bport is input, i2c and eeprom are initianlized as well
	SensorBoardInit();

						// must be done before lifesign and


    //*****************************************************************
	//now start your code here


	SensorBoardSensorsInit();
	SensorBoardSensorsOn();

	ACLInit();											// init the stack and start it

	PRSInit(); //Call after ACLInit !!

	//get_sensor_values(&sensors, PRS_slot_counter%(1<<15));
    //sendEvent(20);

	AppSetLEDBehaviour(LEDS_OFF);
	PCLedRedOn();
	delay_ms(100);
	PCLedRedOff();
	enable_interrupts(global);	


	for(;;)
	{
	  PRSSendSensorValues();
	}
#endif
}



