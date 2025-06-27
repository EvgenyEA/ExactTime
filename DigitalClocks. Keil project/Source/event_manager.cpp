#include "event_manager.h"

EventManager Events;

EventManager::EventManager()
{
	event_list[EVENT_BUTTON_MINUTES].event = EVENT_BUTTON_MINUTES;
	event_list[EVENT_BUTTON_MINUTES].set = 0;
	event_list[EVENT_BUTTON_MINUTES].executed = false;
	event_list[EVENT_BUTTON_MINUTES].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_BUTTON_HOURS].event = EVENT_BUTTON_HOURS;
	event_list[EVENT_BUTTON_HOURS].set = 0;
	event_list[EVENT_BUTTON_HOURS].executed = false;
	event_list[EVENT_BUTTON_HOURS].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_BUZZER].event = EVENT_BUZZER;
	event_list[EVENT_BUZZER].set = 0;
	event_list[EVENT_BUZZER].executed = false;
	event_list[EVENT_BUZZER].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_LED].event = EVENT_LED;
	event_list[EVENT_LED].set = 0;
	event_list[EVENT_LED].executed = false;
	event_list[EVENT_LED].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_TIMER].event = EVENT_TIMER;
  event_list[EVENT_TIMER].set = 0;
  event_list[EVENT_TIMER].executed = false;
  event_list[EVENT_TIMER].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_WDT].event = EVENT_WDT;
  event_list[EVENT_WDT].set = 0;
  event_list[EVENT_WDT].executed = false;
  event_list[EVENT_WDT].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_FLASH].event = EVENT_FLASH;
  event_list[EVENT_FLASH].set = 0;
  event_list[EVENT_FLASH].executed = false;
  event_list[EVENT_FLASH].event_period = INIT_EVENT_PERIOD;
	
	event_list[EVENT_RTC].event = EVENT_RTC;
  event_list[EVENT_RTC].set = 0;
  event_list[EVENT_RTC].executed = false;
  event_list[EVENT_RTC].event_period = INIT_EVENT_PERIOD;
}

/*! -------------------------------------------------------------------------
\brief 			Adds event to manager
\details   	This function set waitng event by receieved number
\param[in] 	event_number
\param[out]	event_nmber if event successfuly set
*/
uint8_t EventManager::AddEvent(const uint8_t event_number, const uint32_t event_period)
{
  uint8_t return_value = 0;
    
  if(event_number < NUMBER_OF_EVENTS)
  {
    event_list[event_number].set = true;
    event_list[event_number].event_period = event_period;
    return_value = event_number;
  }
  
return return_value;
}

/*! -------------------------------------------------------------------------
\brief 		Clear event
\details   	This function clear event by receieved number
\param[in] event_number
\param[out] event_nmber if event successfuly cleared 
*/
uint8_t EventManager::ClearEvent(const uint8_t event_number)
{
  uint8_t return_value = 0;
    
  if(event_number < NUMBER_OF_EVENTS)
  {
    event_list[event_number].set = false;
    event_list[event_number].event_period = INIT_EVENT_PERIOD;
    return_value = event_number;
  }
  
return return_value;
}

/*! -------------------------------------------------------------------------
\brief 			Return waitng event
\details   	This function return number of event from list in priority order
\param[out]	event_nmber if event is set
*/
bool EventManager::GetWaitingEvent(uint8_t &event_number, uint32_t &event_period)
{
		// 1. Check waitng command
	event_number = EVENT_NONE;
	bool waiting_event = false;	
	
	for(uint8_t n = 0; n < NUMBER_OF_EVENTS; n++)
	{
		if((event_list[n].set == true) && (event_list[n].executed != true))
		{
			event_number = event_list[n].event; 
			event_period = event_list[n].event_period;
			event_list[n].executed = true;
			
			waiting_event = true;
			break;
		}
	}
	return waiting_event;
}

/*! -------------------------------------------------------------------------
\brief 			Clear executed events
\details   	This function clear executed flag of all events
\param[out]	
*/
bool EventManager::ClearExecutedEvents()
{
	for(uint8_t n = 0; n < NUMBER_OF_EVENTS; n++)
	{
		event_list[n].executed = false; 
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief 			Clear all events
\details   	This function clear executed flag of all events
\param[out]	
*/
bool EventManager::ClearAllEvents()
{
	for(uint8_t n = 0; n < NUMBER_OF_EVENTS; n++) {
		Events.ClearEvent(n);
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief 			Checks if event is active
\details   	
*/
bool EventManager::CheckIfActive(uint8_t event_number)
{
	if(event_list[event_number].set)
		return true;
	return false;
}

/*! -------------------------------------------------------------------------
\brief 	        Change period
\details   	
*/
bool EventManager::ChangeEventPeriod(const uint8_t event_number, const uint32_t new_event_period)
{
  bool function_failed = true;
    
  if(event_number < NUMBER_OF_EVENTS) {
    event_list[event_number].event_period = new_event_period;
    function_failed = false;
  }
  
  return function_failed;
}
