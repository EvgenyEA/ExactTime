#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief PWM timer for buzzer

\details PWM timer for buzzer
Bus frequency equal to 64 MHz. Divide frequency to 10 kHz (one tick = 0.1 ms) by brescaler. 
TIMx->ARR register set number of milliseconds. With 0.1 ms tick precision of delay is higher. 
*/
void Periphery::InitTim1Pwm()
{
	RCC->APBENR2 |= RCC_APBENR2_TIM1EN;								// Clock enabled	

	// Comman timer settings
    
  TIM1->PSC = 0;               																	// 
  TIM1->ARR =  255;          																		// timer reload 
	//TIM1->DIER |= TIM_DIER_UDE;																		// Update interrupt enable
	//TIM1->CR2 |= TIM_CR2_CCDS;																			// 3 - 1 CCx DMA requests sent when update event occurs/ 0 - CCx DMA request sent when CCx event occurs
	TIM1->CNT = 0;
	
//	TIM1->CR1 &= ~TIM_CR1_ARPE;
	TIM1->CR1 |= TIM_CR1_ARPE;         														// TIMx_ARR register is buffered            
	TIM1->CCER |= TIM_CCER_CC2P;																		// Channel output 1: OC1 active low
	
	// Channel 1 settings
	TIM1->CCMR1 &= ~TIM_CCMR1_OC2M_0;
  TIM1->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);					// OC1M[2:0]: 110 - PWM mode 1 - in upcounting, channel is active as long as TIMx_CNT<TIMx_CCR1 else inactive
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE;																// OC1PE: output compare 2 preload enable
	TIM1->CCMR1 &= ~TIM_CCMR1_CC2S;																// CC1S: Capture/Compare 2 selection | 00: CC3 channel is configured as output
  TIM1->CCR2 = 10; 																						// To check PWM work
	
	//#warning "Seems to be work for DMA2"
	TIM1->DIER |= TIM_DIER_CC2DE;																	// CC1 DMA request enabled
	TIM1->BDTR |= TIM_BDTR_MOE + 12;																// Main output enable + dead time
	TIM1->CCER |= TIM_CCER_CC2E;																		// Capture/Compare 2 output enable
	
	TIM1->CR1 |= TIM_CR1_CEN;
}

void Periphery::UpdateTimPWM(uint16_t reload_value, uint16_t count_value)
{
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM1->ARR = reload_value;
	TIM1->CCR2 = count_value;
	TIM1->CNT = 0;
	TIM1->CCER |= TIM_CCER_CC2E;
	TIM1->CR1 |= TIM_CR1_CEN;
}

void Periphery::StopTimPWM()
{
	TIM1->CCER &= ~TIM_CCER_CC2E;
	TIM1->EGR |= TIM_EGR_UG;	
	TIM1->CR1 &= ~TIM_CR1_CEN;
}
