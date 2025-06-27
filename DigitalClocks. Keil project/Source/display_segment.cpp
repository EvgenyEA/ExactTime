#include "display_segment.h"
#include "periphery.h" 
#include "parameters.h" 
#include "fsm.h" 
#include "button.h" 

SevenSegmentDisplay Display;

uint16_t display_data[DISPLAY_DATA_SIZE];

SevenSegmentDisplay::SevenSegmentDisplay()
{
	postion_counter = 0;
}


/*! -------------------------------------------------------------------------
\brief Update display data
\details  
*/
bool SevenSegmentDisplay::UpdateDisplay()
{
	bool function_failed = false;
	
	uint8_t digit_0 = IrParamWords.ds3231_hours / 10;
	uint8_t digit_1 = IrParamWords.ds3231_hours % 10;
	uint8_t digit_2 = IrParamWords.ds3231_minutes / 10;
	uint8_t digit_3 = IrParamWords.ds3231_minutes % 10;
	uint8_t digit_colon = 255;
	if(IrParamWords.ds3231_seconds % 2) {
		digit_colon = 10;		// colon ON
	}
	else {
		digit_colon = 11;		// colon OFF
	}
	
	switch(postion_counter) {
		case SEGMENT_0:				SetDigit(digit_0, SEGMENT_0);						break;
		case SEGMENT_1:				SetDigit(digit_1, SEGMENT_1);						break;
		case SEGMENT_2:				SetDigit(digit_2, SEGMENT_2);						break;		
		case SEGMENT_3:				SetDigit(digit_3, SEGMENT_3);						break;
		case SEGMENT_COLON:		SetDigit(digit_colon, SEGMENT_COLON);		break;
	}
	
	if(postion_counter < POSITION_MAX) {
		postion_counter++;
	}
	else {
		postion_counter = 0;
	}
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Show digit on 7 segment display
\details  
*/
bool SevenSegmentDisplay::SetDigit(const uint8_t digit, const uint8_t position)
{
	bool function_failed = false;
	if(digit > DIGIT_MAX || position > POSITION_MAX) {
		function_failed = true;
	}
	else {
		uint8_t digit_code = 0;
		uint8_t position_code = 0;
		
		switch(position) {
			case SEGMENT_0:			position_code = SEGMENT_0_REG_CODE;			break;		
			case SEGMENT_1:			position_code = SEGMENT_1_REG_CODE;			break;
			case SEGMENT_2:			position_code = SEGMENT_2_REG_CODE;			break;		
			case SEGMENT_3:			position_code = SEGMENT_3_REG_CODE;			break;
			case SEGMENT_COLON:	position_code = SEGMENT_COLON_REG_CODE;	break;
		}
		
		switch(digit) {
			case 0: 	digit_code = DIGIT_0_REG_CODE; 			break;
			case 1:		digit_code = DIGIT_1_REG_CODE; 			break;
			case 2:		digit_code = DIGIT_2_REG_CODE; 			break;
			case 3:		digit_code = DIGIT_3_REG_CODE; 			break;
			case 4:		digit_code = DIGIT_4_REG_CODE; 			break;
			case 5:		digit_code = DIGIT_5_REG_CODE; 			break;
			case 6:		digit_code = DIGIT_6_REG_CODE; 			break;
			case 7:		digit_code = DIGIT_7_REG_CODE;			break;
			case 8:		digit_code = DIGIT_8_REG_CODE; 			break;
			case 9:		digit_code = DIGIT_9_REG_CODE; 			break;
			case 10:	digit_code = DIGIT_COLON_REG_CODE;	break;
			case 11:	digit_code = 0xFF;									break;
		}
		
		if(Fsm.GetCurrentState() == STATE_TIME) {
			blink_counter++;
			if(blink_counter == MAX_BLINK_COUNTER) {
				blink_counter = 0;
			}

			if(blink_counter > (MAX_BLINK_COUNTER - MAX_BLINK_COUNTER/5)) {
				if(Buttons.CheckLastPress() == BUTTON_HOURS) {
					if(position == SEGMENT_0 || position == SEGMENT_1) {
						digit_code = DIGIT_NONE_CODE;
					}
				}
				else if(Buttons.CheckLastPress() == BUTTON_MINUTES) {
					if(position == SEGMENT_2 || position == SEGMENT_3) {
						digit_code = DIGIT_NONE_CODE;
					}
				}
			}
			
			if(position == SEGMENT_COLON) {
				digit_code = DIGIT_NONE_CODE;
			}

		}
		else if(Fsm.GetCurrentState() == STATE_BRIGHTNESS) {
			if(position == SEGMENT_COLON) {
				digit_code = DIGIT_COLON_REG_CODE;
			}
		}
		
		uint8_t data[2] = {position_code, digit_code};
		Phy.SPI1Write(data, 2);
	}
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief 
\details  
*/
void SevenSegmentDisplay::ClearBlinkCounter() {
	blink_counter = 0;
}