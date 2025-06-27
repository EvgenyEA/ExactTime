#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

const uint32_t FLASH_PARAM_ADDRESS = 0x08007800;
const uint8_t FLASH_DATA_SIZE = 24;
const uint8_t ILLUM_ARRAY_SIZE = 6;

const uint8_t MAX_HOUR 		= 24;
const uint8_t MAX_MINUTE 	= 60;
const uint8_t MAX_SECOND	= 60;
const uint8_t MIN_DATE		= 0;
const uint8_t MAX_DATE		= 32;
const uint8_t MIN_MONTH 	= 1;
const uint8_t MAX_MONTH 	= 12;
const uint8_t MAX_YEAR		= 100;

// Masks and shifts
const uint8_t HOURS_SHIFT = 8;
const uint16_t HOURS_MSK = 0xFF00;
const uint16_t MINUTES_MSK = 0x00FF;

const uint8_t SECONDS_IN_MINUTE = 60;

const uint16_t ILLUMINATION_HISTERESIS = 7;
const uint16_t ILLUMINATION_THRESHOLD = 170; // 165 for original BPW34;


typedef enum day_t {
	MONDAY 		= 1,
	TUESDAY 	= 2,
	WEDNESDAY = 3,
	THURSDAY 	= 4, 
	FRIDAY 		= 5,
	SATURDAY 	= 6,
	SUNDAY 		= 7
} day_t;

// Device words (registers) parameters
typedef	struct IrWords
	{
		uint16_t device_type;													// 0
		uint16_t device_number;												// 1
		uint16_t program_verstion;										// 2 
		uint16_t ds3231_temperature;									// 3
		uint16_t ds3231_seconds;											// 4
		uint16_t ds3231_minutes;											// 5
		uint16_t ds3231_hours;												// 6
		uint16_t ds3231_day;													// 7
		uint16_t ds3231_date;													// 8
		uint16_t ds3231_month;												// 9
		uint16_t ds3231_years;												// 10
		uint16_t brightness;													// 11
		uint16_t illumination[ILLUM_ARRAY_SIZE];			// 12
		uint16_t illumination_average;
		
	} IrWords;

extern IrWords IrParamWords;
const uint8_t PARAM_IR = 12;

class DataManager
{
	public:
		bool InitParameters();
		uint16_t last_device_sate;

	private:
		bool CheckCorrectValue(uint16_t hours_minutes);
};

extern DataManager Parameters; 
	
#endif
