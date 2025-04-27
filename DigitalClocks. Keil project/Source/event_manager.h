#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <stdint.h>
#include "main.h" 

const uint8_t EVENT_NONE	  					=	0;
const uint8_t EVENT_BUTTON_MINUTES		=	1;
const uint8_t EVENT_BUTTON_HOURS			= 2;
const uint8_t EVENT_LED		 						= 3;
const uint8_t EVENT_POWER_VOLTAGE			= 4;
const uint8_t EVENT_BUZZER						= 5; 
const uint8_t EVENT_TIMER							= 6;
const uint8_t EVENT_WDT								= 7;
const uint8_t EVENT_FLASH							= 8;
const uint8_t EVENT_RTC								= 9;

const uint8_t	NUMBER_OF_EVENTS		= 10;		// + 1 (zero event)


struct events
{
	uint32_t event;				// there could be some memory problems if uint8_t
	bool set;
	bool executed; 
	uint32_t event_period; 
};

class EventManager
{
	public:
		EventManager();
		uint8_t AddEvent(const uint8_t event_number, uint32_t event_period);
		bool ClearAllEvents();
		uint8_t ClearEvent(const uint8_t event_number);
		bool GetWaitingEvent(uint8_t &event_number, uint32_t &event_period);
		bool ClearExecutedEvents();
		bool CheckIfActive(const uint8_t event_number);
		bool ChangeEventPeriod(const uint8_t event_number, const uint32_t new_event_period);
	
	private:
		events event_list[NUMBER_OF_EVENTS];
		static const uint16_t INIT_EVENT_PERIOD = 1000;
		
};

extern EventManager Events;

#endif
