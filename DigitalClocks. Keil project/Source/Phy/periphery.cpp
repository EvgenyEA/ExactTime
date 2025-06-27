#include "periphery.h"

Periphery Phy;

/*! -------------------------------------------------------------------------
\brief Periphery initialization
*/
void Periphery::InitPhy()
{
	InitTick();
	InitGPIO();
	InitSPI1();
	//InitDMACh1();
	InitI2C1();
	InitTim1Pwm();
	InitTim3Ms();
	InitTim14DisplayBrightness();
	InitTim16Buzzer();
	InitTim17DisplayUpdate();	
	InitSystemTimer();
	InitADC();
	InitInterrupts();
	InitDMACh2();
}

/*! -------------------------------------------------------------------------
\brief Microseconds delay function
*/
bool Periphery::DelayUs(uint16_t delay)
{
	bool function_failed = true;
	for(uint32_t n = 0; n < delay; n++) {
		asm("nop"); asm("nop"); asm("nop");  
	}
	return function_failed;
}

