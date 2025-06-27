#include "periphery.h" 


/*! -------------------------------------------------------------------------
\brief DMA channel 1 initialization function
\param[out] 
*/
void Periphery::InitDMACh2()
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;												// enable DMA1 clock
	
	DMAMUX1_Channel1->CCR = 21;															// TIM1 CH2 (!!! Channel X DMAMUX = channe DMA - 1)
  DMA1_Channel2->CCR = 0;    															// clear configuration register	
  DMA1_Channel2->CMAR = (uint32_t) sound_sin;							// memory address
	DMA1_Channel2->CPAR = (uint32_t)&(TIM1->CCR2);  				// periphery address	
	DMA1_Channel2->CNDTR = 100;												      // number of data to transfer
	DMA1_Channel2->CCR &= ~DMA_CCR_MEM2MEM;									// from memory to periphery
  DMA1_Channel2->CCR |= DMA_CCR_MINC;											// memory increment mode enabled
  DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE;										// 00: memory size 8-bits
	DMA1_Channel2->CCR &= ~DMA_CCR_PINC;										// periphery increment disabled
	DMA1_Channel2->CCR |= DMA_CCR_PSIZE_0; 									// PSIZE 16-bit
	DMA1_Channel2->CCR |= DMA_CCR_CIRC;											// 1: Circular mode enabled
	DMA1_Channel2->CCR |= DMA_CCR_DIR;											// data transfer direction 1: read from memory
	DMA1_Channel2->CCR |= DMA_CCR_PL;												// channel priority level 11: very hight
	
	DMA1_Channel2->CCR |= DMA_CCR_TCIE;											// transfer complete interrupt enable

	//DMA1_Channel2->CCR |= DMA_CCR1_EN;										// channel enabled
}

/*! -------------------------------------------------------------------------
\brief Is transaction through DMA complete
*/
bool Periphery::CheckDMACh2State()
{
  if(DMA1->ISR & DMA_ISR_TCIF2) return 0; 			
  return 1;      																					// transaction in progress
}

/*! -------------------------------------------------------------------------
\brief Start DMA trnasmission
*/
void Periphery::StartDMACh2(const uint16_t data_length)
{
  DMA1_Channel2->CCR  &= ~DMA_CCR_EN;      								// forbid to work with channel
  DMA1->IFCR = DMA_IFCR_CTCIF2;  										      // clear end of transmition flag
	DMA1_Channel2->CNDTR = data_length;	

	TIM1->CR1 |= TIM_CR1_CEN;
  DMA1_Channel2->CCR |= DMA_CCR_EN;												// channel enabled	
}

/*! -------------------------------------------------------------------------
\brief Stop DMA trnasmission
*/
void Periphery::StopDMACh2()
{
	DMA1_Channel2->CCR  &= ~DMA_CCR_EN;;      								// forbid to work with channel
  DMA1->IFCR = DMA_IFCR_CTCIF2;  										// clear end of transmition flag
	
	TIM1->EGR |= TIM_EGR_UG;
//	TIM1->BDTR &= ~TIM_BDTR_MOE;
	TIM1->CR1 &= ~TIM_CR1_CEN;
}
