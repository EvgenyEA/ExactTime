#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief Real time clock initialization function
\details  
*/
bool Periphery::InitRTC()
{
	bool function_failed = true; 
	
	RCC->CSR |= RCC_CSR_LSION;								// Enable LSI
	
	for(uint16_t n = 0; n < 60000; n++)				// Wait for flag
	{
		if(RCC->CSR & RCC_CSR_LSIRDY) {
			function_failed = false;
			break;
		}
	}
	RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN;
	RCC->APBENR1 |= RCC_APBENR1_PWREN;				// Tick for PWR
	PWR->CR1 |= PWR_CR1_DBP;										// Clear protection from RTC registers
	RCC->BDCR &= ~RCC_BDCR_RTCSEL_0;
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;					// Set RTC clock from LSI(32 kHz) )
	RCC->BDCR |= RCC_BDCR_RTCEN;							// Enable tick of RTC
	
	RTC->WPR = 0xCA;													// Clear RTC write protection
	RTC->WPR = 0x53;
	
	RTC->ICSR |= RTC_ICSR_INIT;									// Go to initializte mode
	for(uint16_t n = 0; n < 60000; n++)
	{
		if(RTC->ICSR & RTC_ICSR_INITF)
		{
			function_failed = false;
			break;
		}
	}
	
	RTC->CR = 0;	
	
	uint16_t prediv_s   = (LSI_FREQUENCY / 5) - 1;	
	RTC->PRER = (0x04 << RTC_PRER_PREDIV_A_Pos) | (prediv_s << RTC_PRER_PREDIV_S_Pos);
	
	RTC->ICSR &= ~(RTC_ICSR_INIT);							// Exit initialization mode
	RTC->WPR = 0xFF;													// Enable protection
		
	return function_failed;
}



/*! -------------------------------------------------------------------------
\brief Set RTC interrupt every second 
\details
*/
bool Periphery::RTCInterruptEnable()
{
		bool function_failed = true;
	
	RTC->WPR = 0xCA;																			// Clear RTC write protection
	RTC->WPR = 0x53;
	
	RTC->CR &=~ RTC_CR_WUTE; 															// Disable wake up timer to modify it 
	
	for(uint16_t n = 0; n < 60000; n++)
	{
		if(!((RTC->SR & RTC_SR_WUTF) != RTC_SR_WUTF)) 	// Wait until it is allow to modify wake up reload value
		{
			function_failed = false;
			break;
		}
	}
                                 
	RTC->CR &= ~RTC_CR_WUCKSEL_0;
	RTC->CR &= ~RTC_CR_WUCKSEL_1;
	RTC->CR |= RTC_CR_WUCKSEL_2;													// "ck_spre" (1 Hz) selected as a clock
	RTC->WUTR = 59;
	
	RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE; 								// Enable wake up counter and wake up interrupt
	
	RTC->CR &= ~RTC_CR_OSEL_Msk;													// Disable output
	RTC->SR &= ~(RTC_SR_WUTF);
	
	RTC->WPR = 0xFE; 
	RTC->WPR = 0x64; 																			// Disable write access 
	
	return function_failed;
}

