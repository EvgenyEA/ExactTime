#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief SPI1 initialization
\details 
*/
void Periphery::InitSPI1()
{
	// Настройка порта SPI для управления радиомодулем
	RCC->APBENR2 |= RCC_APBENR2_SPI1EN; 							// SPI1 clock enabled  
	
	// Ножки порта настраиваются в функции инициализации GPIO
	
	SPI1->CR1 |= SPI_CR1_MSTR;              					// Mode Master
	
	SPI1->CR1 |= SPI_CR1_BR_0;                					// Baud rate 110:Fpclk/128
	SPI1->CR1 |= SPI_CR1_BR_1;												// 000: Fpclk/2, 001 Fpclk/4
	SPI1->CR1 |= SPI_CR1_BR_2;

	
  SPI1->CR1 &= ~SPI_CR1_CPOL;             					// Polarity cls signal CPOL = 0;
  SPI1->CR1 &= ~SPI_CR1_CPHA;             					// Phase cls signal    CPHA = 0;


//	SPI1->CR2 |= SPI_CR2_DS_0;												// SPI data length 0x07 - 8 bit
//	SPI1->CR2 |= SPI_CR2_DS_1;	
//	SPI1->CR2 &= ~SPI_CR2_DS_2;	
//	SPI1->CR2 |= SPI_CR2_DS_3;	
//	SPI1->CR2 |= SPI_CR2_FRXTH;	// RXNE event is generated if the FIFO level is greater than or equal to 1/4

	
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST;         		// MSB will be first
//	SPI1->CR1 |= SPI_CR1_LSBFIRST;         		// MSB will be first
	
  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  		// Software slave management & Internal slave select
//	SPI1->CR2 = SPI_CR2_SSOE | SPI_CR2_RXNEIE;
//SPI1->CR2 |= SPI_CR2_SSOE;									// Enable NSS output

//	SPI1->CR2 |= SPI_CR2_TXDMAEN;								// Tx buffer DMA enable
	
  SPI1->CR1 |= SPI_CR1_SPE;                		// Enable SPI1
}

/*! -------------------------------------------------------------------------
\brief Send data to SPI
\details 
*/
bool Periphery::SPI1Write(const uint8_t data_send)
{
	// Флаг TXE равен единице, в случае, если FIFO пуст
	if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) 	// Test Tx empty
	{
		*(uint8_t *)&(SPI1->DR) = data_send;						// Will inititiate 8-bit transmission if TXE 
	}
	else { return 1; }
	
	// Ожидаем окончания отправки данных
	while(SPI1->SR & SPI_SR_BSY);

	if ((SPI1->SR & SPI_SR_TXE) == 0) { return 1; }
	
	// Считываем данные, очищаем RXFIFO. Данные не интеесуют
	uint8_t data_receive = 0;
	if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	{
		data_receive = (*(uint8_t *)&(SPI1->DR)); // receive data, clear flag 
	}	
	
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Send data to SPI
\details 
*/
bool Periphery::SPI1Write(const uint8_t *data_send, const uint16_t data_lenght)
{
	bool function_failed = false;
	
	for(uint16_t n = 0; n < data_lenght; n++) {
		if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) 	// Test Tx empty
		{
			*(uint8_t *)&(SPI1->DR) = data_send[n];						// Will inititiate 8-bit transmission if TXE 
		}
		while(SPI1->SR & SPI_SR_BSY);
		
		// Считываем данные, очищаем RXFIFO. Данные не интеесуют
		uint8_t data_receive = 0;
		if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
		{
			data_receive = (*(uint8_t *)&(SPI1->DR)); // receive data, clear flag 
		}
	}	

}


/*! -------------------------------------------------------------------------
\brief Send data to SPI, get data from SPI
\details 
*/
bool Periphery::SPI1Write(uint8_t data_send, uint8_t &data_receive)
{
	// Флаг TXE равен единице, в случае, если FIFO пуст
	if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) 	// Test Tx empty
	{
		*(uint8_t *)&(SPI1->DR) = data_send;						// Will inititiate 8-bit transmission if TXE 
	}
	else { return 1; }
	
	// Ожидаем окончания отправки данных
	while(SPI1->SR & SPI_SR_BSY);

	if ((SPI1->SR & SPI_SR_TXE) == 0) { return 1; }
	
		// Считываем данные, очищаем RXFIFO
	if ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	{
		data_receive = (*(uint8_t *)&(SPI1->DR)); // receive data, clear flag 
	}

	return 0;
}
