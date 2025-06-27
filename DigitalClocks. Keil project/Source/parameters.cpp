#include "parameters.h" 
#include "periphery.h"

IrWords IrParamWords;

DataManager Parameters; 

/*! -------------------------------------------------------------------------
\brief Parameters initialization
\param[out] 
*/
bool DataManager::InitParameters()
{
	bool function_failed = false;
	
	// internal variables
	uint8_t flash_read[FLASH_DATA_SIZE];
	function_failed |= Phy.FlashRead(FLASH_PARAM_ADDRESS, flash_read, FLASH_DATA_SIZE);
	if(flash_read[0] != 0 && flash_read[7] == 0xEA) {
		IrParamWords.brightness = flash_read[0];
	}
	else {
		IrParamWords.brightness = 0xFF;
	}
	
	// IR initialization
	IrParamWords.device_type      = 0x0C;
	IrParamWords.device_number    = 1;
	IrParamWords.program_verstion = 1;	
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Check correctness of value
*/
bool DataManager::CheckCorrectValue(uint16_t hours_minutes)
{
	if(((((hours_minutes & HOURS_MSK) >> HOURS_SHIFT) < MAX_HOUR) && (hours_minutes & MINUTES_MSK) < MAX_MINUTE)) 
		return true;
	return false;
}
