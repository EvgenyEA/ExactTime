#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

typedef enum last_button_t
{
	BUTTON_UNDEFINED,
	BUTTON_HOURS,
	BUTTON_MINUTES
} last_button_t;



typedef enum button_status_t
{
  BUTTON_NONE							= 0,
	BUTTON_WAIT   					= 1,
	BUTTON_HOURS_SHORT				= 2,
	BUTTON_HOURS_HALF_LONG 	= 3,
	BUTTON_HOURS_LONG 				= 4,
	BUTTON_MINUTES_SHORT 			= 5,
	BUTTON_MINUTES_HALF_LONG 	= 6,
	BUTTON_MINUTES_LONG 			= 7,
	BUTTONS_1_AND_2_LONG 		= 8,
	BUTTON_NOT_VALID				= 9
} button_status_t;

const uint8_t BUTTON_STATE_FILTER = 0;
const uint8_t BUTTON_STATE_RELEASE = 1;

const uint32_t FILTER_TIME_MS = 40;
const uint32_t BUTTON_HALF_LONG_MS = 1500;
const uint32_t BUTTON_LONG_MS = 4000;


/*! -------------------------------------------------------------------------
\brief Class for button 1
*/

class Button_class
{
	public:
		Button_class();
		void ButtonHoursPressed();
		void ButtonMinutesPressed();
		button_status_t Check();
		last_button_t CheckLastPress();
	
	private:
		uint32_t button_hours_start_time;
		bool button_hours_pressed;
		bool button_hours_half_long;
	
		uint32_t button_minutes_start_time;
		bool button_minutes_pressed;
		bool button_minutes_half_long;
	
		last_button_t last_button_pressed;
};

extern Button_class Buttons;




#endif
