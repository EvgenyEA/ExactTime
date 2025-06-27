#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief Tick settings
*/
bool Periphery::InitTick()
{
	bool function_failed = true; 
				
	RCC->CFGR &= ~RCC_CFGR_SWS_Msk;						// SystemClock selected from internal HSI RC
	RCC->CR |= RCC_CR_HSION;									// HSI RC ON (16 MHz)
	RCC->CR |= RCC_CR_HSIDIV_0;								// 001: divide by 2

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_1;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_0;		// HSI as clock source
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;		// 0: PLLM = 1
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_4;			
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;				// 011: PLLR = 4
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_1;				// 1: PLLR = 2
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	
	RCC->CR |= RCC_CR_PLLON;									// PLL Enabled
	
	for(uint32_t n = 0; n < 100000; n++) {
		if(RCC->CR & RCC_CR_PLLRDY) {
			function_failed = false;
			break;
		}
	}
	
	FLASH->ACR &= ~FLASH_ACR_LATENCY_0;
	FLASH->ACR |= FLASH_ACR_LATENCY_1;				// Flash memory access latency 010: Two wait states
	
	RCC->CFGR &= ~RCC_CFGR_SWS_Msk;
	RCC->CFGR |= RCC_CFGR_SW_1;
	
	// Output frequency is equal to 64 MHz
	
	return function_failed;
}
