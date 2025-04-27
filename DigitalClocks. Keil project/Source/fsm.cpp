#include "fsm.h"
#include "event_manager.h" 


State Fsm;

const uint32_t _100_MS_TIME = 99;
const uint32_t _1000_MS_TIME = 999;


/*! -------------------------------------------------------------------------
\brief Constructor
*/
State::State()
{
	set_state = STATE_WORK;
  current_state = STATE_UNDEFINED;            // Current state is undefined 
}

/*! -------------------------------------------------------------------------
\brief State machine
\details  This function do steps between states
\param[out] change of state
*/
bool State::UpdateState()
{
	bool change_of_state = false;
	
	switch(set_state)
  {
		case STATE_UNDEFINED:
		case STATE_WORK: 
			if(current_state != STATE_WORK) 
			{
				current_state = STATE_WORK;
				SetStateWork();
				change_of_state = true;
			}
		 break;
				
		case STATE_TIME:
			if(current_state != STATE_TIME)
			{
				current_state = STATE_TIME;
				SetStateTime();
				change_of_state = true;
			}
			break;

		
		case STATE_BRIGHTNESS:
			if(current_state != STATE_BRIGHTNESS)
			{
				current_state = STATE_BRIGHTNESS;
				SetStateBrightness();
				change_of_state = true;
			}
			break;
			
    // The "default" section must not be here. If device in one of the states, 
		// function just do nothing.  
  }
	return change_of_state;
}


/*! -------------------------------------------------------------------------
\brief Change of the state
*/
void State::ChangeState(states_t new_state)
{
	if(current_state != new_state) {		
		current_state = STATE_UNDEFINED;
		set_state = new_state;
	}
}

/*! -------------------------------------------------------------------------
\brief Check current state of device
*/
states_t State::GetCurrentState()
{
	return current_state;
}



/*! -------------------------------------------------------------------------
\brief Set WORK state
*/
void State::SetStateWork()
{
	Events.ClearAllEvents();
	Events.AddEvent(EVENT_BUZZER, _100_MS_TIME);
	Events.AddEvent(EVENT_POWER_VOLTAGE, _1000_MS_TIME);
	Events.AddEvent(EVENT_WDT, 0);
	Events.AddEvent(EVENT_RTC, _1000_MS_TIME);
	Events.AddEvent(EVENT_LED, _1000_MS_TIME);
	Events.AddEvent(EVENT_TIMER, _1000_MS_TIME);
}

/*! -------------------------------------------------------------------------
\brief Set TIME state
*/
void State::SetStateTime()
{

}

/*! -------------------------------------------------------------------------
\brief Set BRIGHTHNESS state
*/
void State::SetStateBrightness()
{

}
