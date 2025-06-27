// Autor: Emelyanov E.A.
// Digital Clocks the Exact Time
#include "main.h"
#include "periphery.h" 
#include "device.h" 
#include "buzzer.h" 
#include "fsm.h"

int main()
{
	Phy.InitPhy();
	Buzzer.InitBuzzer();
	Device.InitDevice();
	
	while(1)
	{		
		Fsm.UpdateState();
		Device.CheckWaitingEvent();
	}
}

