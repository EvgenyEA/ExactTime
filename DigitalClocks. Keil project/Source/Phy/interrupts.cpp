#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief Interrupts initialization
\param[out] 
*/
void Periphery::InitInterrupts()
{
	RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
	
	// PA0 - BUTTTON MINUTES
	EXTI->EXTICR[0] &= ~EXTI_EXTICR1_EXTI0_Msk;					// PA on EXTI0
	EXTI->IMR1 |= EXTI_IMR1_IM0;
//	EXTI->EMR1 |= EXTI_EMR1_EM0;											// Event request is not masked (for wake up from stop)
  EXTI->RTSR1 |= EXTI_RTSR1_RT0;											// Rising trigger selection
  EXTI->RPR1 = EXTI_RPR1_RPIF0;
	
	NVIC_SetPriority(EXTI0_1_IRQn, 2);
	NVIC_EnableIRQ (EXTI0_1_IRQn);
	
	// PA12 - BUTTON HOURS
	EXTI->EXTICR[3] &= ~EXTI_EXTICR4_EXTI12_Msk;				// PA on EXTI12
	EXTI->IMR1 |= EXTI_IMR1_IM12;
//	EXTI->EMR1 |= EXTI_EMR1_EM12;											// Event request is not masked (for wake up from stop)
  EXTI->RTSR1 |= EXTI_RTSR1_RT12;											// Rising trigger selection
  EXTI->RPR1 = EXTI_RPR1_RPIF12;
	
	NVIC_SetPriority(EXTI4_15_IRQn, 2);
	NVIC_EnableIRQ (EXTI4_15_IRQn);
	
	// Buzzer note duration timer
	NVIC_EnableIRQ(TIM16_IRQn);	
	NVIC_SetPriority(TIM16_IRQn,2);
	
	// Dispaly update timer
	NVIC_EnableIRQ(TIM17_IRQn);	
	NVIC_SetPriority(TIM17_IRQn, 1);
	
	// DMA1 interrupt
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);	
	NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
}
