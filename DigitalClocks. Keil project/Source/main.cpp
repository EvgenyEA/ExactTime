// File made 16.06.22
// Autor: Emelyanov E.A.
// Programm for Digital Clocks
#include "main.h"
#include "periphery.h" 
#include "device.h" 
#include "buzzer.h" 
#include "notes.h" 
#include "display_segment.h"
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

