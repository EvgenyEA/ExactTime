#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief Timer in ms for pump
\details   Initialize timer to count with 1 milliseconds
Bus frequency equal to 64 MHz. Divide frequency to 1 kHz (one tick = 0.1 ms) by brescaler. 
TIMx->ARR register set number of milliseconds. With 0.1 ms tick precision of delay is higher. 
*/
void Periphery::InitTim17DisplayUpdate()
{	
	RCC->APBENR2 |= RCC_APBENR2_TIM17EN;	
//	TIM17->CR1 |= TIM_CR1_ARPE;										// Auto reload enabled
	TIM17->CR1 |= TIM_CR1_URS;										// Reload without interrupts
	TIM17->SR = 0; 																// Clear flags  
  TIM17->PSC = 63999;							  						// Prescaler 
	TIM17->ARR = 1;																// AutoReload Register 
	TIM17->CNT = 0;
	TIM17->DIER |= TIM_DIER_UIE;									// Allow interrupt update	

	TIM17->CR1 |= TIM_CR1_CEN;						    		// Start the timer counter 	
}

/*! -------------------------------------------------------------------------
\brief Set new timer value
\details   
*/
//void Periphery::StartTIM17Pump(uint16_t time_ms)
//{
//	TIM17->CR1 &= ~TIM_CR1_CEN;
//	TIM17->ARR = time_ms;
//	TIM17->CCR1 = 0;
//	TIM17->CNT = 0;
//	TIM17->EGR |= TIM_EGR_UG;											// Reinitialize the counter and generates an update of the registers
////	TIM17->CR1 |= TIM_CR1_URS;										// Reload without interrupts
//	TIM17->SR &= ~TIM_SR_UIF;
//	TIM17->CR1 |= TIM_CR1_CEN;
//}

/*! -------------------------------------------------------------------------
\brief Stop timer
*/
//void Periphery::StopTIM17Pump()
//{
//	TIM17->CR1 &= ~TIM_CR1_CEN;						    		// Start the timer counter 	
//}
