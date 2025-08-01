#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief Timer for buzzer control
\details   Initialize timer to milliseconds
Bus frequency equal to 64 MHz. Divide frequency to 10 kHz (one tick = 0.1 ms) by brescaler. 
TIMx->ARR register set number of milliseconds. With 0.1 ms tick precision of delay is higher. 
*/
void Periphery::InitTim16Buzzer()
{	
	RCC->APBENR2 |= RCC_APBENR2_TIM16EN;	
	TIM16->CR1 |= TIM_CR1_ARPE;										// Auto reload enabled
	TIM16->CR1 |= TIM_CR1_URS;										// Reload without interrupts
	TIM16->SR = 0; 																// Clear flags  
  TIM16->PSC = 6399;							  							// Prescaler 
	TIM16->ARR = 9;																// AutoReload Register 
	TIM16->CNT = 0;
	TIM16->DIER |= TIM_DIER_UIE;									// Allow interrupt update	

	//TIM16->CR1 |= TIM_CR1_CEN;						    		// Start the timer counter 	
}

/*! -------------------------------------------------------------------------
\brief Set new timer value
\details   
*/
bool Periphery::UpdateTIM16(uint16_t reload_value_ms)
{
	if(reload_value_ms < 6500) {
		TIM16->CR1 &= ~TIM_CR1_CEN;
		TIM16->ARR = reload_value_ms * 10;
		TIM16->CCR1 = 0;
		TIM16->CNT = 0;
		TIM16->EGR |= TIM_EGR_UG;											// Reinitialize the counter and generates an update of the registers
	//	TIM16->CR1 |= TIM_CR1_URS;										// Reload without interrupts
		TIM16->CR1 |= TIM_CR1_CEN;
		return 0;
	}
	return 1;
}

/*! -------------------------------------------------------------------------
\brief Stop timer
*/
void Periphery::StopTIM16()
{
	TIM16->CR1 &= ~TIM_CR1_CEN;						    		// Start the timer counter 	
}
