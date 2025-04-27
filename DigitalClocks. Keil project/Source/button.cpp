#include "button.h" 
#include "periphery.h" 
#include "device.h" 

Button_class Buttons;

/*! -------------------------------------------------------------------------
\brief Constructor
*/
Button_class::Button_class()
{
	button_hours_pressed = false;
	button_minutes_pressed = false;
	
	button_hours_half_long = false;
	button_minutes_half_long = false;
	
	last_button_pressed = BUTTON_UNDEFINED;
}

/*! -------------------------------------------------------------------------
\brief Fix press time of the button1
*/
void Button_class::ButtonMinutesPressed()
{
	button_minutes_start_time = Device.GetGlobalTime();
	button_minutes_half_long = false;
	button_minutes_pressed = true;
	last_button_pressed = BUTTON_MINUTES;
}

/*! -------------------------------------------------------------------------
\brief Fix press time of the button2
*/
void Button_class::ButtonHoursPressed()
{
	button_hours_start_time = Device.GetGlobalTime();
	button_hours_half_long = false;
	button_hours_pressed = true;
	last_button_pressed = BUTTON_HOURS;
}

/*! -------------------------------------------------------------------------
\brief
*/
last_button_t Button_class::CheckLastPress()
{
	return last_button_pressed;
}

/*! -------------------------------------------------------------------------
\brief Wait till filter time expire
*/
button_status_t Button_class::Check()
{
	button_status_t status_return;
	button_status_t status_button_open = BUTTON_NONE;
	button_status_t status_button_close = BUTTON_NONE;
	
	if(button_hours_pressed) {
		uint32_t delta_time1 = Device.GetGlobalTime() - button_hours_start_time;
		
		if(delta_time1 < FILTER_TIME_MS)
		{
			if(BUTTON_HOURS_STATE) { status_button_open = BUTTON_WAIT; }		// button shoud be pressed during filter time
			else { 
			status_button_open = BUTTON_NOT_VALID;	
			}
		}
		else if(delta_time1 >= FILTER_TIME_MS  && delta_time1 < BUTTON_HALF_LONG_MS)
		{
			if(BUTTON_HOURS_STATE)	{	status_button_open = BUTTON_WAIT;	}		// if still pressed, it may be long, let's wait
			else { status_button_open = BUTTON_HOURS_SHORT; }
		}
		else if(delta_time1 >= BUTTON_HALF_LONG_MS  && delta_time1 < BUTTON_LONG_MS)
		{
			if(button_hours_half_long == false) {
				if(BUTTON_HOURS_STATE)	{	
					button_hours_half_long = true;
					status_button_open = BUTTON_HOURS_HALF_LONG;	
				}	
				else { status_button_open = BUTTON_HOURS_SHORT; }
			}
			else {
				if(BUTTON_HOURS_STATE)	{	status_button_open = BUTTON_WAIT;	}		// if still pressed, it may be long, let's wait
				else { status_button_open = BUTTON_NOT_VALID; }
			}
		}
		else if(delta_time1 >= BUTTON_LONG_MS)
		{
			status_button_open = BUTTON_HOURS_LONG;
		}
		else
		{
			status_button_open = BUTTON_NOT_VALID;
		}
		
		if((status_button_open == BUTTON_NOT_VALID) || status_button_open != BUTTON_WAIT) {
			if(status_button_open != BUTTON_HOURS_HALF_LONG) {
				button_hours_pressed = false;
				button_hours_half_long = false;
			}
		}
	}
	
	
	if(button_minutes_pressed) {
		uint32_t delta_time2 = Device.GetGlobalTime() - button_minutes_start_time;
		
		if(delta_time2 < FILTER_TIME_MS)
		{
			if(BUTTON_MINUTES_STATE) { status_button_close = BUTTON_WAIT; }		// button shoud be pressed during filter time
			else { status_button_close = BUTTON_NOT_VALID; }
		}
		else if(delta_time2 >= FILTER_TIME_MS  && delta_time2 < BUTTON_HALF_LONG_MS)
		{
			if(BUTTON_MINUTES_STATE)	{	status_button_close = BUTTON_WAIT;	}		// if still pressed, it may be long, let's wait
			else { status_button_close = BUTTON_MINUTES_SHORT; }
		}
		else if(delta_time2 >= BUTTON_HALF_LONG_MS  && delta_time2 < BUTTON_LONG_MS)
		{
			if(button_minutes_half_long == false) {
				if(BUTTON_MINUTES_STATE)	{	
					button_minutes_half_long = true;
					status_button_close = BUTTON_MINUTES_HALF_LONG;	
				}	
				else { status_button_close = BUTTON_MINUTES_SHORT; }
			}
			else {
				if(BUTTON_MINUTES_STATE)	{	status_button_close = BUTTON_WAIT;	}		// if still pressed, it may be long, let's wait
				else { status_button_close = BUTTON_NOT_VALID; }
			}
		}
		else if(delta_time2 >= BUTTON_LONG_MS)
		{
			status_button_close = BUTTON_MINUTES_LONG;
		}
		else
		{
			status_button_close = BUTTON_NOT_VALID;
		}
		
		if((status_button_close == BUTTON_NOT_VALID) || status_button_close != BUTTON_WAIT) {
			if(status_button_close != BUTTON_MINUTES_HALF_LONG) {
				button_minutes_pressed = false;
				button_minutes_half_long = false;
			}
		}
	}
	
	if(status_button_open && status_button_close == BUTTON_NONE) {
		if(status_button_open == BUTTON_HOURS_LONG) {
			if(BUTTON_MINUTES_STATE) {
				status_return = BUTTONS_1_AND_2_LONG;
			}
			else {
				status_return = status_button_open;
			}
		}
		else {
			status_return = status_button_open;
		}
	}
	else if(status_button_close && status_button_open == BUTTON_NONE) {
		if(status_button_close == BUTTON_MINUTES_LONG) {
			if(BUTTON_HOURS_STATE) {
				status_return = BUTTONS_1_AND_2_LONG;
			}
			else {
				status_return = status_button_close;
			}
		}
		else {
			status_return = status_button_close;
		}
	}
	else if(status_button_open && status_button_close) {
		if(status_button_open == BUTTON_WAIT) {
			status_return = BUTTON_WAIT;
		}
		else if(status_button_open == BUTTON_HOURS_HALF_LONG) {
			status_return = BUTTON_HOURS_HALF_LONG;
		}
		else if(status_button_open == BUTTON_HOURS_LONG) {
			status_return = BUTTONS_1_AND_2_LONG;
		}
	}
	
	return status_return;
}


