void AppSetLEDBehaviour(int ledstyle)
{
        PCLedRedOff();
        PCLedBlueOff();
	rf_LED_style=ledstyle;
        AppSetLEDs();
}


// this function is called at slot end and handles the leds
#separate
void AppSetLEDs()
{
  if (rf_LED_style==LEDS_OFF) //Do not touch leds in OFF-mode
  {
    return;
  }
  else
  {
    PCLedBlueOff();
    PCLedRedOff();
  }

  if (rf_LED_style==LEDS_ON_CRC_ERROR)
  {
    switch (LL_last_data)
    {
      case LL_GOOD:
      case LL_ACM:
        PCLedBlueOn();
        PCLedRedOff();
        break;
      case LL_CRC_ERROR:
        PCLedBlueOn();
        PCLedRedOn();
        break;
      default:
        PCLedBlueOff();
        PCLedRedOff();
        break;
    }
  }
  else
  {
    if (bit_test(rf_slotcounter,1)) PCLedRedOn(); else PCLedRedOff();				// this is for alife sign "blinking"
  }

  if (rf_LED_style==LEDS_NORMAL)
  {
#ifdef SYNC_STATE_SYNCED
    if (rf_sync_state==SYNC_STATE_SYNCED) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
#else
    // PCLedBlueOn();
#endif
  }
  if (rf_LED_style==LEDS_ON_RECEIVE)
  {
    if (LLDataIsNew()) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
  }
  if (rf_LED_style==LEDS_ON_SEND)
  {
    if (ACLSentPacketInThisSlot()) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
  }


}

