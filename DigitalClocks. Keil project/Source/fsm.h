#ifndef FSM_H
#define FSM_H

#include <stdint.h>

// States
typedef enum states_t {
	STATE_UNDEFINED,
	STATE_WORK, 
	STATE_TIME,
	STATE_BRIGHTNESS
} states_t;


class State
{
  public:
    State();  
    bool UpdateState();
    void ChangeState(states_t);
		states_t GetCurrentState();
	
		states_t current_state; 
		
  private:
    void SetStateWork();
    void SetStateTime();
		void SetStateBrightness();
    
    states_t set_state;
		uint8_t wake_up_source;
};

extern State Fsm;

#endif
