#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief Timer to tune the brightness of 7 segment display
\details   
Bus frequency equal to 64 MHz. Divide frequency by brescaler. 
TIMx->ARR register set reload value. 
*/
void Periphery::InitTim14DisplayBrightness()
{
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;														// enable clock

	// Comman timer settings
//  TIM14->CR1 |= TIM_CR1_ARPE;         														// TIMx_ARR register is buffered              
  TIM14->PSC = 7;               																	// 
  TIM14->ARR =  255;          																		// timer reload 
	//TIM14->DIER |= TIM_DIER_UDE;																		// Update interrupt enable
	//TIM14->CR2 |= TIM_CR2_CCDS;																			// 3 - 1 CCx DMA requests sent when update event occurs/ 0 - CCx DMA request sent when CCx event occurs
	TIM14->CNT = 0;
	
	TIM14->CR1 &= ~TIM_CR1_ARPE;
	TIM14->CCER |= TIM_CCER_CC1P;																		// Channel output 1: OC1 active low
	
	// Channel 1 settings
	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
  TIM14->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);					// OC1M[2:0]: 110 - PWM mode 1 - in upcounting, channel is active as long as TIMx_CNT<TIMx_CCR1 else inactive
	TIM14->CCMR1 |= TIM_CCMR1_OC1PE;																// OC1PE: output compare 2 preload enable
	TIM14->CCMR1 &= ~TIM_CCMR1_CC1S;																// CC1S: Capture/Compare 2 selection | 00: CC3 channel is configured as output
  TIM14->CCR1 = 255; 																						// To check PWM work
	
	//#warning "Seems to be work for DMA2"
	//TIM14->DIER |= TIM_DIER_CC1DE;																	// CC1 DMA request enabled
	//TIM14->BDTR |= TIM_BDTR_MOE + 12;																// Main output enable + dead time
	TIM14->CCER |= TIM_CCER_CC1E;																		// Capture/Compare 1 output enable
}

/*! -------------------------------------------------------------------------
\brief Tune 7 segment brightness
\details   
*/
bool Periphery::SetDisplayBrightness(const uint8_t brightness)
{
	if(brightness == 255) {
		TIM14->EGR |= TIM_EGR_UG;
	//	TIM14->CCER &= ~TIM_CCER_CC1E;
		TIM14->CR1 &= ~TIM_CR1_CEN;
	}
	else {
		TIM14->CR1 &= ~TIM_CR1_CEN;
		//TIM14->ARR = reload_value;
		TIM14->CCR1 = brightness;
		TIM14->CNT = 0;
		TIM14->CCER |= TIM_CCER_CC1E;
		TIM14->CR1 |= TIM_CR1_CEN;
	}
}

//void Periphery::StopTimPWM()
//{
//	TIM14->CCER &= ~TIM_CCER_CC1E;
//	TIM14->CR1 &= ~TIM_CR1_CEN;
//}

