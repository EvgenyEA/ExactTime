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
	InitDMACh1();
	
//	InitRTC();
	
	InitI2C1();
	InitTim3Ms();
	InitTim1Pwm();
	InitTim14DisplayBrightness();
	InitTim16Buzzer();
	InitTim17DisplayUpdate();	
	

//	InitPowerMode();
	InitSystemTimer();
	InitADC();
	InitInterrupts();
					//	InitTemperatureSensor();
//	InitIWDT();

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

