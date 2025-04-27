#include "button.h" 
#include "periphery.h" 
#include "device.h" 

Button1_class ButtonMinutes;
Button2_class ButtonHours;


/*! -------------------------------------------------------------------------
\brief Fix press time
*/
void Button1_class::Pressed()
{
	start_time = Device.GetGlobalTime();
}


/*! -------------------------------------------------------------------------
\brief Wait till filter time expire
*/
uint8_t Button1_class::Check()
{
	uint8_t status;
	
	uint32_t delta_time = Device.GetGlobalTime() - start_time;
	if(delta_time < FILTER_TIME_MS)
	{
		status = BUTTON_PRESS_WAIT; 		// button should be pressed during filter time
	}
	else if(delta_time >= FILTER_TIME_MS  && delta_time < BUTTON_LONG_WAIT_MS)
	{
		if(BUTTON_MINUTES_STATE)	{	status = BUTTON_PRESS_WAIT;	}		// if still pressed, it may be long, let's wait
		else { status = BUTTON_PRESS_SHORT; }
	}
	else if(delta_time >= BUTTON_LONG_WAIT_MS)
	{
		if(BUTTON_MINUTES_STATE) {
			status = BUTTON_PRESS_LONG;
		}
		else {
			status = BUTTON_PRESS_NOT_VALID;
		}
	}
	else
	{
		status = BUTTON_PRESS_NOT_VALID;
	}
	
	return status;
}

/*! -------------------------------------------------------------------------
\brief Fix press time
*/
void Button2_class::Pressed()
{
	start_time = Device.GetGlobalTime();
}

/*! -------------------------------------------------------------------------
\brief Wait till filter time expire
*/
uint8_t Button2_class::Check()
{
	uint8_t status;
	
	uint32_t delta_time = Device.GetGlobalTime() - start_time;
	if(delta_time < FILTER_TIME_MS)
	{
		status = BUTTON_PRESS_WAIT; 		// button should be pressed during filter time
	}
	else if(delta_time >= FILTER_TIME_MS  && delta_time < BUTTON_LONG_WAIT_MS)
	{
		if(BUTTON_HOURS_STATE)	{	status = BUTTON_PRESS_WAIT;	}		// if still pressed, it may be long, let's wait
		else { status = BUTTON_PRESS_SHORT; }
	}
	else if(delta_time >= BUTTON_LONG_WAIT_MS)
	{
		if(BUTTON_HOURS_STATE) {
			status = BUTTON_PRESS_LONG;
		}
		else {
			status = BUTTON_PRESS_NOT_VALID;
		}
	}
	else
	{
		status = BUTTON_PRESS_NOT_VALID;
	}
	
	return status;
}

