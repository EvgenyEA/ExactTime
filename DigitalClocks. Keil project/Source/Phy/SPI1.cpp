#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief SPI initialization
\details 
*/
void Periphery::InitSPI1()
{
	RCC->APBENR2 |= RCC_APBENR2_SPI1EN; 
	
	SPI1->CR1 |= SPI_CR1_MSTR;              					              // Mode Master
	SPI1->CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2);      // Baud rate
  SPI1->CR1 &= ~SPI_CR1_CPOL;             					              // Polarity cls signal CPOL = 0;
  SPI1->CR1 &= ~SPI_CR1_CPHA;             					              // Phase cls signal    CPHA = 0;
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST;         		                    // MSB will be first
  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  		                    // Software slave management & Internal slave select
	
  SPI1->CR1 |= SPI_CR1_SPE;  
}

/*! -------------------------------------------------------------------------
\brief Send data to SPI
\details 
*/
bool Periphery::SPI1Write(const uint8_t data_send)
{
	if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) { 	                  // check Tx empty
		*(uint8_t *)&(SPI1->DR) = data_send;						              // will inititiate 8-bit transmission
	}
	else { return 1; }
	
	while(SPI1->SR & SPI_SR_BSY);
	
	uint8_t data_receive = 0;
	if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) 	{
		data_receive = (*(uint8_t *)&(SPI1->DR));                     // receive data, clear flag 
	}	
	
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Send data to SPI
\details 
*/
bool Periphery::SPI1Write(const uint8_t *data_send, const uint16_t data_lenght)
{

	for(uint16_t n = 0; n < data_lenght; n++) {
		if((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) {	
			*(uint8_t *)&(SPI1->DR) = data_send[n];						          // will inititiate 8-bit transmission 
		}
		else { return 1; }
		
		while(SPI1->SR & SPI_SR_BSY);
		
		uint8_t data_receive = 0;
		if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) {
			data_receive = (*(uint8_t *)&(SPI1->DR));                   // receive data, clear flag 
		}
	}	

	return 0;
}


/*! -------------------------------------------------------------------------
\brief Send data to SPI, get data from SPI
\details 
*/
bool Periphery::SPI1Write(uint8_t data_send, uint8_t &data_receive)
{
	if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) {
		*(uint8_t *)&(SPI1->DR) = data_send;						             // will inititiate 8-bit transmission 
	}
	else { return 1; }
	
	while(SPI1->SR & SPI_SR_BSY);

	if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE) {
		data_receive = (*(uint8_t *)&(SPI1->DR));                    // receive data, clear flag 
	}
	return 0;
}
