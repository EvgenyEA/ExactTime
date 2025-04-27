#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief Power mode initialization
\details Set Range 3 power mode
*/
void Periphery::InitPowerMode()
{
	RCC->APBENR1 |= RCC_APBENR1_PWREN;
	FLASH->ACR &= ~(FLASH_ACR_LATENCY_0 | FLASH_ACR_LATENCY_2);
	FLASH->ACR |= FLASH_ACR_LATENCY_1;														// Flash memory access latency

//	PWR->CR1 |= (0x02 << PWR_CR1_VOS_Pos);													// Voltage scaling range 2
	PWR->CR1 |= PWR_CR1_FPD_LPSLP;
	PWR->CR1 |= PWR_CR1_FPD_STOP;																	// Flash memory powered down during Stop mode
	PWR->CR1 &= ~(PWR_CR1_LPMS_Msk);
	PWR->CR1 |= PWR_CR1_LPMS_0;																		// Stop mode 1
}

/*! -------------------------------------------------------------------------
\brief Go to Stop mode
*/
void Periphery::EnterStopMode()
{	
	
//	PWR->CR |= PWR_CR_CWUF;										// Clear WUF Wakeup flag after 2 system clock cycles
	
	PWR->CR1 |= PWR_CR1_LPR;																			// regulator is switched from main mode (MR) to low-power mode (LPR)
	
//	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;			// After waking up on interrupt go to sleep again	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 				// Go to Stop mode after WFI instruction
//	SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;				// Only enalbed inerrupts could wake up CPU
			
	__dsb(15); 
	__WFI(); 
	__isb(15); 
}
