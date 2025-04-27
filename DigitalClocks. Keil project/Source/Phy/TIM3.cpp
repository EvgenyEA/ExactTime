#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief Timer 1 for milliseconds delay

\details   Function tune timer for millisecond delay
Bus frequency equal to 48 MHz. Divide frequency to 10 kHz (one tick = 0.1 ms) by brescaler. 
TIMx->ARR register set number of milliseconds. With 0.1 ms tick precision of delay is higher. 
*/
void Periphery::InitTim3Ms()
{
	RCC->APBENR1 |= RCC_APBENR1_TIM3EN;								// Clock enabled	
	TIM3->PSC = 4799; 
	TIM3->CR1 |= TIM_CR1_URS;													// Update without interrupt generation 
	TIM3->SR = 0; 																		// Clear interrupt flag
}

/*! -------------------------------------------------------------------------
\brief Millisecond delay function

\details  Tick of the timer is equal to 10 kHz, one tick = 1 ms.
Timer register size is equal to 32 bitc, max count 4294967296. Function limit
max falue to 60000 ms. 
\param[in] 	delay
\param[out] status
*/
bool Periphery::DelayMs(uint16_t delay)
{
	bool function_failed = true;
	if(delay < 1 || delay > 60000) { return 1; }
	TIM3->ARR = delay * 10 - 1;												// Записываем значение в регистр перезагрузки
	TIM3->EGR |= TIM_EGR_UG;													// Очищаем счетчный регистр
	TIM3->CR1 |= TIM_CR1_CEN;													// Запускаем счет
	for(uint32_t n = 0; n < 10000000; n++)						// 10000000 = 2.5 sec
	{
		if(TIM3->SR & TIM_SR_UIF) {
			function_failed = false;
			break;
		}
	}

	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->SR &= ~TIM_SR_UIF;													// Очищаем флаг переполнения
	return function_failed;
}


/*! -------------------------------------------------------------------------
\brief Initialize tim in PWM mode
*/
/*
void Periphery::InitTim3Pwm()
{
	RCC->APBENR1 |= RCC_APBENR1_TIM3EN;														// enable clock

	// Comman timer settings
//  TIM3->CR1 |= TIM_CR1_ARPE;         													// TIMx_ARR register is buffered              
  TIM3->PSC = 1;   
  TIM3->ARR =  2024;          																	// timer reload 
	//TIM3->DIER |= TIM_DIER_UDE;																	// Update interrupt enable
	//TIM3->CR2 |= TIM_CR2_CCDS;																	// 3 - 1 CCx DMA requests sent when update event occurs/ 0 - CCx DMA request sent when CCx event occurs
	TIM3->CNT = 0;
	
	TIM3->CR1 &= ~TIM_CR1_ARPE;
	TIM3->CCER |= TIM_CCER_CC1P;																	// Channel output 1: OC1 active low
	
	// Channel 1 settings
	TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;
  TIM3->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);					// OC1M[2:0]: 110 - PWM mode 1 - in upcounting, channel is active as long as TIMx_CNT<TIMx_CCR1 else inactive
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;																// OC1PE: output compare 2 preload enable
	TIM3->CCMR1 &= ~TIM_CCMR1_CC1S;																// CC1S: Capture/Compare 2 selection | 00: CC3 channel is configured as output
  TIM3->CCR1 = 1012; 																						// To check PWM work
	
	//#warning "Seems to be work for DMA2"
	//TIM3->DIER |= TIM_DIER_CC1DE;																// CC1 DMA request enabled
	//TIM3->BDTR |= TIM_BDTR_MOE + 12;														// Main output enable + dead time
	//TIM3->CCER |= TIM_CCER_CC1E;																	// Capture/Compare 1 output enable
	
}

void Periphery::UpdateTimPWM(uint16_t reload_value, uint16_t count_value)
{
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->ARR = reload_value;
	TIM3->CCR1 = count_value;
	TIM3->CNT = 0;
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void Periphery::StopTimPWM()
{
	TIM3->CCER &= ~TIM_CCER_CC1E;
	TIM3->CR1 &= ~TIM_CR1_CEN;
}
*/
