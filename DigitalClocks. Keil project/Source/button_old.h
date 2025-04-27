#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

//const uint8_t BUTTON_NONE = 0;
//const uint8_t BUTTON_CENTER = 1;
//const uint8_t BUTTON_RIGHT = 2;
//const uint8_t BUTTON_LEFT = 3;

const uint8_t BUTTON_PRESS_WAIT = 0;
const uint8_t BUTTON_PRESS_SHORT = 1;
const uint8_t BUTTON_PRESS_LONG = 2;
const uint8_t BUTTON_PRESS_NOT_VALID = 3;

const uint8_t BUTTON_STATE_FILTER = 0;
const uint8_t BUTTON_STATE_RELEASE = 1;

const uint32_t FILTER_TIME_MS = 80;
const uint32_t BUTTON_LONG_WAIT_MS	= 350;


/*! -------------------------------------------------------------------------
\brief Class for button 1
*/

class Button1_class
{
	public:
		void Pressed();
		uint8_t Check();
		
	private:
		uint32_t start_time;
};

extern Button1_class ButtonMinutes;


/*! -------------------------------------------------------------------------
\brief Class for button 3
*/
class Button2_class
{
	public:
		void Pressed();
		uint8_t Check();
		
	private:
		uint32_t start_time;
};

extern Button2_class ButtonHours;

#endif
