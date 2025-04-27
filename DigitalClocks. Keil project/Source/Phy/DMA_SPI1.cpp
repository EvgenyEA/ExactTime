#include "periphery.h"
#include "display_segment.h" 


/*! -------------------------------------------------------------------------
\brief DMA channel 1 initialization function
\param[out] 
*/
void Periphery::InitDMACh1()
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;												// enable DMA1 clock
	
	DMAMUX1_Channel0->CCR = 17;															// SPI1_TX (!!! Channel X DMAMUX = channe DMA - 1)
	
  DMA1_Channel1->CCR = 0;    															// clear configuration register
	
  DMA1_Channel1->CMAR = (uint32_t)&display_data;					// memory address
	DMA1_Channel1->CPAR = (uint32_t)&(SPI1->DR);  					// periphery address	
	
  DMA1_Channel1->CNDTR = DISPLAY_DATA_SIZE;								// number of data to transfer
//	DMA1_Channel1->CNDTR = 1;															// number of data to transfer
	DMA1_Channel1->CCR &= ~DMA_CCR_MEM2MEM;									// from memory to periphery
 // DMA1_Channel1->CCR |= DMA_CCR_MINC;											// memory increment mode enabled
	
 // DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE;										// 00: memory size 8-bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; 									// PSIZE 16-bit
	
	DMA1_Channel1->CCR &= ~DMA_CCR_PINC;										// periphery increment disabled
	//DMA1_Channel1->CCR |= DMA_CCR_CIRC;											// 1: Circular mode enabled
	DMA1_Channel1->CCR &= ~DMA_CCR_CIRC;											// 1: Circular mode enabled
	DMA1_Channel1->CCR |= DMA_CCR_DIR;											// data transfer direction 1: read from memory
	DMA1_Channel1->CCR |= DMA_CCR_PL;												// channel priority level 11: very hight
	
	//DMA1_Channel1->CCR |= DMA_CCR_HTIE;
	DMA1_Channel1->CCR |= DMA_CCR_TCIE;											// transfer complete interrupt enable

	DMA1_Channel1->CCR |= DMA_CCR_EN;												// channel enabled
}

/*! -------------------------------------------------------------------------
\brief Is transaction through DMA complete
*/
bool Periphery::CheckDMACh1State()
{
  if(DMA1->ISR & DMA_ISR_TCIF1) return 0; 									// transaction complete channel 1
  return 1;      																						// transaction in progress
}

/*! -------------------------------------------------------------------------
\brief Start DMA trnasmission
*/
void Periphery::StartDMACh1(uint16_t data_length)
{
	
  DMA1_Channel1->CCR  &= ~DMA_CCR_EN;;      								// forbid to work with channel
  DMA1_Channel1->CNDTR =  data_length;      								// set amount of data to transmit
  DMA1->IFCR       =  DMA_IFCR_CTCIF1;  										// clear end of transmition flag

  DMA1_Channel1->CCR |= DMA_CCR_EN;												// channel enabled	
}
