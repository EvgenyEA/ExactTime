// ----------------------------------------------------------
// Library to work with I2C for STM32F0
// Made 29.10.2018. Autor: Emalyanov E.A.  
// ----------------------------------------------------------
#include "periphery.h" 

uint16_t protection_counter = 1000;

/*! -------------------------------------------------------------------------
\brief I2C initialization
\details Input frequency 8 MHZ, on this frequency relay I2C TRIMING register
(see I2C timing configuration tool for STM32  AN4235 xls). 
*/
bool Periphery::InitI2C1()
{
	bool function_failed = false;
	
	RCC->APBENR1 |= RCC_APBENR1_I2C1EN;												// Enable I2C tick
	
	I2C1->CR1 &= ~I2C_CR1_PE;																	// Disable I2C
	uint16_t cnt1 = protection_counter;
	while (I2C1->CR1 & I2C_CR1_PE) 
	{
		cnt1++;
		if(cnt1 == 0)
		{
			function_failed = true;
			break;
		}
	};	
	
	I2C1->CR1 &= ~I2C_CR1_ANFOFF;															// Disable analog filter
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;													// Disable clock stretching
	
/*
	I2C1->TIMINGR |= (I2C_TIMINGR_SCLL & 0x13);
	I2C1->TIMINGR |= (I2C_TIMINGR_SCLH & (0x0F << I2C_TIMINGR_SCLH_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_SDADEL & (0x00 << I2C_TIMINGR_SDADEL_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_SCLDEL & (0x01 << I2C_TIMINGR_SCLDEL_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_PRESC & (0x0B << I2C_TIMINGR_PRESC_Pos));
*/

//	// 400 kHz @ 8MHz tick
//	I2C1->TIMINGR|= (I2C_TIMINGR_SCLL & 0x08);
//	I2C1->TIMINGR|= (I2C_TIMINGR_SCLH & (0x03 << I2C_TIMINGR_SCLH_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_SDADEL & (0x00 << I2C_TIMINGR_SDADEL_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_SCLDEL & (0x01 << I2C_TIMINGR_SCLDEL_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_PRESC & (0x00 << I2C_TIMINGR_PRESC_Pos));
	
//	// 100 kHz @ 64 MHz tick
//	I2C1->TIMINGR|= (I2C_TIMINGR_SCLL & 251);
//	I2C1->TIMINGR|= (I2C_TIMINGR_SCLH & (61 << I2C_TIMINGR_SCLH_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_SDADEL & (0 << I2C_TIMINGR_SDADEL_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_SCLDEL & (6 << I2C_TIMINGR_SCLDEL_Pos));
//	I2C1->TIMINGR |= (I2C_TIMINGR_PRESC & (1 << I2C_TIMINGR_PRESC_Pos));

	// 400 kHz @ 64 MHz tick
	I2C1->TIMINGR|= (I2C_TIMINGR_SCLL & 108);
	I2C1->TIMINGR|= (I2C_TIMINGR_SCLH & (33 << I2C_TIMINGR_SCLH_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_SDADEL & (0 << I2C_TIMINGR_SDADEL_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_SCLDEL & (12 << I2C_TIMINGR_SCLDEL_Pos));
	I2C1->TIMINGR |= (I2C_TIMINGR_PRESC & (0 << I2C_TIMINGR_PRESC_Pos));

	
	
	// I2C1->CR1 |= I2C_CR1_RXIE; 														// Enable reciver interupt
	// I2C1->CR1 |= I2C_CR1_TCIE;															// Enable tranciver interrupt
	// I2C1->CR2 |= I2C_CR2_AUTOEND;													// Master send STOP condition automatically, when NBYTES[7:0] were transmitted
	// I2C_BUS->CR2 &= ~I2C_CR2_RELOAD;												// Do not use reload mode
	
	I2C1->CR1 |= I2C_CR1_PE;																	// Enable I2C
	uint16_t cnt2 = protection_counter;
	while (!(I2C1->CR1 & I2C_CR1_PE))
	{
		cnt2++;
		if(cnt2 == 0)
		{
			function_failed = true;
			break;
		}
	};
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief I2C settings
\details Function to control I2C data flow. Function sets parameters and generates start condition.
Address mode 7 bit. 
\param 
direction (0 - transmitter, 1 - receiver)
size (from 0 to 255)
*/
bool Periphery::I2C_StartDirectionAddressSize (I2C_Direction Direction, uint8_t Address, uint8_t Size)
{	
	bool function_failed = false;
	
	if (Direction) 
	{ 
		I2C1->CR2 |= I2C_CR2_RD_WRN;													// Receiver
	}	
	else 
	{
		I2C1->CR2 &= ~I2C_CR2_RD_WRN;													// Transmitter
	}

	I2C1->CR2 &= ~I2C_CR2_NBYTES;														// Clear data size
	I2C1->CR2 |= Size << I2C_OFFSET_CR2_NBYTES;							// Set data size
	
	I2C1->CR2 &= ~I2C_CR2_SADD;															// Clear slave address
	I2C1->CR2 |= Address;																		// Slave address
	
	I2C1->CR2 |= I2C_CR2_START;															// Generate start condition
	
	uint16_t cnt1 = protection_counter;
	while(!I2C1->ISR & I2C_ISR_BUSY)
	{
		cnt1--;
		if(cnt1 == 0)
		{
			function_failed = true;
			break;
		}
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief I2C Stop
\details
*/
bool Periphery::StopI2C1()
{
	bool function_failed = false;
	I2C1->CR2 |= I2C_CR2_STOP;				// Выдать стоп на шину
	
	uint16_t cnt1 = protection_counter;
	while (I2C1->ISR & I2C_ISR_BUSY) 
	{
		cnt1--;
		if(cnt1 == 0) 
		{ 
			function_failed = true; 
			break; 
		}
	};		// Ожидать выдачу стопа
	// Очищаю флаги - необходимо для дальнейшей работы шины
	I2C1->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	I2C1->ICR |= I2C_ICR_NACKCF;		// NACK флаг
	// Если есть ошибки на шине - очищаю флаги
	if (I2C1->ISR & (I2C_ISR_ARLO | I2C_ISR_BERR))
	{
		I2C1->ICR |= I2C_ICR_ARLOCF;
		I2C1->ICR |= I2C_ICR_BERRCF;
		function_failed = true; 										// Имеется какая-то ошибка
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief I2C reset
\details Программный сброс выполняется выключением и последующим включением
модуля I2C. В этом случае линии SCL и SDA освобождаются, внутренняя 
машина состояния сбрасывается а статусные биты возвращаются к значениям
по умолчанию. 
*/
void Periphery::ResetI2C1()
{
	I2C1->CR1 &= ~I2C_CR1_PE;	
	for(uint8_t n = 0; n < 250; n++);
	I2C1->CR1 |= I2C_CR1_PE;	
}

/*! -------------------------------------------------------------------------
\brief Data transmition
\details Выполняет транзакцию записи Size байт в регистр Register по адресу Adress.
\param[out] device_address - address of slave device
						data_address - two bytes of addrss space
						data - array of data to send
						size - number of bytes to send
*/
bool Periphery::WriteI2C1_DS3231(const uint8_t device_address, const uint8_t data_command, uint8_t *data, const uint8_t data_amount)
{
	if(data_amount == 0xFF) {
		return 1;
	}
	
	uint8_t data_count = 0;																																// counter of transmitted bytes
	bool function_failed = false;

	// 1. Start condition and settings
	function_failed = I2C_StartDirectionAddressSize (I2C_Transmitter, device_address, data_amount + 1);			// data amount + address 

	uint16_t cnt1 = protection_counter;
	while(!(I2C1->ISR & I2C_ISR_TXIS))
	{
		cnt1--;
		if(cnt1 == 0) 
		{ 
			function_failed = true; 
			break; 
		}
	}
	*(uint8_t *)&(I2C1->TXDR) = data_command;
	
	
	// 3. Send data
	if(data_amount) {
		uint16_t cnt2 = protection_counter;
		while (!(I2C1->ISR & I2C_ISR_TC) && !(I2C1->ISR & I2C_ISR_NACKF))
		{
			cnt2--;
			if(cnt2 == 0)
			{
				function_failed = true;
				break;
			}
			if (I2C1->ISR & I2C_ISR_TXIS) {
				I2C1->TXDR =*(data + data_count++);										// send data
				cnt2 = protection_counter;
			}
		}
	}
	
	// 4. Generate stop condition
	function_failed |= StopI2C1();
	
	if (data_count != data_amount) { function_failed = true; }
		
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Data reception
\details Function to read data from slave device
\param[out] device_address - address of slave device
						data_address - two bytes of addrss space
						data - array to receive data
						size - number of bytes to receive
*/
bool Periphery::ReadI2C1_DS3231(const uint8_t device_address, const uint8_t data_command, uint8_t *data, const uint8_t data_amount)
{
	bool function_failed = false;
	uint8_t data_count = 0;																																// counter of received bytes


	// 1. Start condition and settings
	function_failed = I2C_StartDirectionAddressSize (I2C_Transmitter, device_address, 1);			// data amount + address
	
	
	// 2. Send address
	uint16_t cnt1 = protection_counter;
	while(!(I2C1->ISR & I2C_ISR_TXIS))
	{
		cnt1--;
		if(cnt1 == 0) 
		{ 
			function_failed = true; 
			break; 
		}
	}
	*(uint8_t *)&(I2C1->TXDR) = data_command;
	

	
	/////////////
	
	// 3. Repeat start to prepear for data reception 
	function_failed |= I2C_StartDirectionAddressSize (I2C_Receiver, device_address, data_amount);
	
	// 4. Receive data from slave
	uint16_t cnt4 = 6000;
	while ( (((I2C1->ISR & I2C_ISR_TC) == 0) && ((I2C1->ISR & I2C_ISR_NACKF) == 0)) || (I2C1->ISR & I2C_ISR_RXNE) != 0)
	{
		if (I2C1->ISR & I2C_ISR_RXNE) 
		{	
			*(data + data_count++) = I2C1->RXDR;
			cnt4 = 6000;
		}
		if(cnt4 == 0) 
		{ 
				function_failed = true; 
				break; 
		}
		cnt4--;
	}
	
	// 5. Generate stop condition
	function_failed |= StopI2C1();
	
	if (data_count != data_amount) { function_failed = true; }
		
	return function_failed;
}
