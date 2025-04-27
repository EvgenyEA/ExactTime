#ifndef DISPLAY_SEGMENT
#define DISPLAY_SEGMENT

#include <stdint.h>

const uint8_t DIGIT_0_REG_CODE = 0xFF - 0x5F;
const uint8_t DIGIT_1_REG_CODE = 0xFF - 0x42;
const uint8_t DIGIT_2_REG_CODE = 0xFF - 0x9B;
const uint8_t DIGIT_3_REG_CODE = 0xFF - 0xD3;
const uint8_t DIGIT_4_REG_CODE = 0xFF - 0xC6;
const uint8_t DIGIT_5_REG_CODE = 0xFF - 0xD5;
const uint8_t DIGIT_6_REG_CODE = 0xFF - 0xDD;
const uint8_t DIGIT_7_REG_CODE = 0xFF - 0x43;
const uint8_t DIGIT_8_REG_CODE = 0xFF - 0xDF;
const uint8_t DIGIT_9_REG_CODE = 0xFF - 0xD7;
const uint8_t DIGIT_NONE_CODE = 0xFF;
const uint8_t DIGIT_COLON_REG_CODE = 0xFF - 0x20;

const uint8_t POSITION_MAX = 4;
const uint8_t DIGIT_MAX = 11;

const uint8_t SEGMENT_0_REG_CODE = 0x01;
const uint8_t SEGMENT_1_REG_CODE = 0x02;
const uint8_t SEGMENT_2_REG_CODE = 0x04;
const uint8_t SEGMENT_3_REG_CODE = 0x08;
const uint8_t SEGMENT_COLON_REG_CODE = SEGMENT_1_REG_CODE;

const uint8_t SEGMENT_0 = 0;
const uint8_t SEGMENT_1 = 1;
const uint8_t SEGMENT_2 = 2;
const uint8_t SEGMENT_3 = 3;
const uint8_t SEGMENT_COLON = 4;


const uint8_t DISPLAY_DATA_SIZE = 1;
extern uint16_t display_data[DISPLAY_DATA_SIZE];
const uint8_t DISPLAY_MAX_HOURS = 23;
const uint8_t DISPLAY_MAX_MINUTES = 59;

const uint32_t MAX_BLINK_COUNTER = 500;

class SevenSegmentDisplay
{
	public:
		SevenSegmentDisplay();
		bool UpdateDisplay();
		void ClearBlinkCounter();
	
		bool SetDigit(const uint8_t digit, const uint8_t position);
	
	private:
		uint8_t postion_counter;
		uint8_t current_hours;
		uint8_t current_minutes;
		uint32_t blink_counter;
		
};

extern SevenSegmentDisplay Display;

#endif