#include "periphery.h"

const uint32_t FLASH_KEY1 = 0x45670123; 
const uint32_t FLASH_KEY2 = 0xCDEF89AB;

const uint32_t FLASH_OPTKEY1 = 0x08192A3B; 
const uint32_t FLASH_OPTKEY2 = 0x4C5D6E7F; 

const uint8_t MAX_PAGE_NUMBER = 15;		// 32 kbite by 2 kbite pages from zero

/*! -------------------------------------------------------------------------
\brief Check busy flag
*/
bool Periphery::FlashWaitBusy() 
{
	bool function_filed = false;
	uint32_t protection_counter = 200000;
  while (FLASH->SR & FLASH_SR_BSY1 || FLASH->SR & FLASH_SR_CFGBSY) 
	{
		protection_counter--;
		if(!protection_counter) {
			function_filed = true;
			break;
		}
	}
	return function_filed;
}

/*! -------------------------------------------------------------------------
\brief Unlock memory
*/
bool Periphery::FlashUnlock(void) 
{
    FlashWaitBusy();

    if (FLASH->CR & FLASH_CR_LOCK)  
    {
      FLASH->KEYR = FLASH_KEY1;
      FLASH->KEYR = FLASH_KEY2;

   //   FLASH->OPTKEYR = FLASH_OPTKEY1;
   //   FLASH->OPTKEYR = FLASH_OPTKEY2;
    }  
      
	return FlashWaitBusy();
}

/*! -------------------------------------------------------------------------
\brief Lock memory
*/
bool Periphery::FlashLock() 
{
	bool function_failed = FlashWaitBusy();
	if(!function_failed) {
    FLASH->CR |= FLASH_CR_LOCK;
	}
  function_failed |= FlashWaitBusy();
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Erase memory by pages
*/
bool Periphery::FlashErasePage(uint8_t page_address) 
{
	bool function_failed = true;
	if(page_address <= MAX_PAGE_NUMBER)
	{
		FlashUnlock();
	
		FLASH->SR = 0xFFFF;				// clear errors
		
		if(!FlashWaitBusy()) 
		{
			FLASH->SR = 0xFFFF;
			FLASH->CR |= FLASH_CR_PER; 					// one page
			FLASH->CR |= (page_address << FLASH_CR_PNB_Pos);
			FLASH->CR |= FLASH_CR_STRT; 				// start erase

			for(uint8_t n = 0; n < 100; n++) {
				__NOP();
			}

			FlashWaitBusy();

			if (FLASH->SR & FLASH_SR_EOP)	{
				FLASH->SR = FLASH_SR_EOP;
			}

			FLASH->CR &= ~FLASH_CR_PER;
		}
		FlashLock();
	}
}

/*! -------------------------------------------------------------------------
\brief Mass erase
*/
void Periphery::FlashMassErase(void) 
{
    FlashWaitBusy();
		FLASH->SR |= FLASH_SR_PGSERR;					// clear errors
    FLASH->CR |= FLASH_CR_MER1; 						// mass erase
    FLASH->CR |= FLASH_CR_STRT; 					// start erase

    __NOP();
    __NOP();

    FlashWaitBusy();
	
    if (FLASH->SR & FLASH_SR_EOP)
    {
      FLASH->SR = FLASH_SR_EOP;
    }

    FLASH->CR &= ~FLASH_CR_MER1;
}

/*! -------------------------------------------------------------------------
\brief Write data to memory
\details Flash write operation is performed by half-word
*/
bool Periphery::FlashWrite(uint32_t const address, const uint8_t* data, uint16_t const data_len) 
{
	bool function_falied = true;
	
	if((address % 8 == 0) && (data_len % 8 == 0))		
	{
		uint32_t temp_address = address;
		uint64_t temp_data = 0;	
		uint16_t temp_data_len = data_len / 8;
		FLASH->SR = 0xFFFF;
		 
		if(!FlashUnlock())
		{
			FLASH->SR = 0xFFFF;												// Clear erros
			FLASH->CR |= FLASH_CR_PG; 								// Set the PG bit in the FLASH_CR register to enable programming
			
			FlashWaitBusy();
			
			for(uint16_t n = 0; n < temp_data_len; n++)
			{				
				for(uint8_t byte = 0; byte < 8; byte++) {
					temp_data |= data[7 - byte];
					if(byte < 7) {
					temp_data <<= 8;
					}
				}
				
				*(__IO uint64_t*)(temp_address) = *(uint64_t*)&temp_data;			// Perform the data write (half-word) at the desired address
				
				function_falied = FlashWaitBusy();
				temp_address += 8;
			}
		
			
				FlashWaitBusy();
				if ((FLASH->SR & FLASH_SR_EOP) != 0) /* (4) Check the EOP flag in the FLASH_SR register */
				{
					FLASH->SR = FLASH_SR_EOP; //(5) clear it by software by writing it at 1 */
				}
				else
				{
					function_falied = true;
				}
	
				FLASH->CR &= ~FLASH_CR_PG; /* (6) Reset the PG Bit to disable programming */
				
				FlashLock();
		}
		else
		{
			function_falied = true;
		}
	}
	return function_falied;
}

/*! -------------------------------------------------------------------------
\brief Read data from momory
*/
bool Periphery::FlashRead(uint32_t const address, uint8_t* data, uint16_t const data_len) 
{
	bool function_failed = false;
	if((address % 4 == 0) && (data_len % 4 == 0))		// we can read only 4 bytes of data
	{
		uint32_t temp_data = 0;
		uint32_t temp_address = address;
		uint16_t temp_data_len = data_len/4;
		
    for(uint16_t n = 0; n < temp_data_len; n++) 
    {
			*(uint32_t*)&temp_data = *(volatile uint32_t*)(temp_address);
			temp_address += 4;
			for(uint8_t k = 0; k < 50; k++)
			{
				 __NOP();		// Wait for data read
			}
			data[n * 4] = temp_data;
			data[n * 4 + 1] = (temp_data >> 8); 
			data[n * 4 + 2] = (temp_data >> 16); 
			data[n * 4 + 3] = (temp_data >> 24); 
    }
	}
	else
	{
		function_failed = true;
	}
   
	return function_failed;
}
