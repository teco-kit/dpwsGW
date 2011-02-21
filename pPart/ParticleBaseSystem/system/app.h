//#separate
//void AppRfSlotEndCallBack();

void AppSetRealTimeClock(long value);					//sets the real time clock to given value
int AppGetRealTimeClockH();								//returns the HIGh byte of the real time clock
int AppGetSupplyVoltage();								//returns the supply voltage; 255 means 3.0 V, scaled linear

void AppSetLEDBehaviour(int ledstyle);					//sets how LED should behave (blicking, blue on transmit, sync etc, off)
#separate
void AppSetLEDs();										//internal: updates the state of the LEDs
int AppSelfTest(int* result);										//runs the selftest if a selftest connector is on
