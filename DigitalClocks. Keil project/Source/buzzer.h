#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>


// buzzer states
const uint8_t BUZZER_IDLE = 0;
const uint8_t BUZZER_PLAY = 1;
const uint8_t BUZZER_PAUSE = 2;

class SoundBuzzer
{
	public:
		void InitBuzzer();
		bool PlayNote();
		void ClearValues();

	
	private:


};

extern SoundBuzzer Buzzer;


#endif

