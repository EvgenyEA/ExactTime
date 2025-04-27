#include "periphery.h" 
#include "core_cm0plus.h" 

/*! -------------------------------------------------------------------------
\brief System timer initialization
\details Timer increment happens with MCU clock frequency. If MCU frquency equal 
8000000, then one tick time = 1/8000000 = 0,000000125. To get 1 ms tick, we need count till
0,001/0,000000125 = 8000. It is equal to 8000000/1000. 
*/
void Periphery::InitSystemTimer()
{
	SysTick->LOAD = MCU_FREQUENCY/1000 - 1; 				// Reload every 1 ms
	SysTick->VAL = MCU_FREQUENCY/1000 - 1; 					// Set current value

	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		// Clock source - processor
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;			// Counting down to zero to asserts the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;				// Enable counter
}

/*! -------------------------------------------------------------------------
\brief Start system tick timer
\details  
*/
void Periphery::StartSystemTick()
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*! -------------------------------------------------------------------------
\brief Stop system tick timer
\details  
*/
void Periphery::StopSystemTick()
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
