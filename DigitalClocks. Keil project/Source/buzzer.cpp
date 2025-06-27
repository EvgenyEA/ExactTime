#include "buzzer.h" 
#include "periphery.h" 
#include "notes.h" 
#include "sin_value.h" 

SoundBuzzer Buzzer;

uint16_t notes_dots[48];
uint8_t sound_sin[500];
uint8_t A = 127;
uint8_t step = 1;
uint16_t selected_note_dotes = 1;

uint8_t fading = 127;


// This array contain precalculated values for PWM tim to generate note frequency 
// DO			DO#			RE			RE#			MI			FA			FA#			SOL			SOL#		LA			LA#			SI
uint16_t tone[] = {
	30581,	28860,	27248,	25707,	24272,	22910,	21622,	20408,	19259,	18182,	17160,	16194,
	15291,	14430,	13619,	12858,	12136,	11455,	10811,	10204,	9632,		9091,		8580,		8099,
	7645,		7215,		6811,		6428,		6067,		5727,		5405,		5102,		4816,		4545,		4290,		4049,
	3822,		3608,		3405,		3214,		3034,		2863,		2703,		2551,		2408,		2273,		2145,		2025
};


/*! -------------------------------------------------------------------------
\brief 
*/
void SoundBuzzer::InitBuzzer()
{
	for(uint8_t n = 0; n < 48; n++) {
		notes_dots[n] = (MCU_FREQUENCY/256)/NOTES_FRQ[n];
	}
	
	selected_note_dotes = notes_dots[N_a2];		//N_fis2 max loudness, N_eis2 - good, N_a2 - nice
	step = SIN_DOTS/selected_note_dotes;
}


/*! -------------------------------------------------------------------------
\brief Return current state of the buzzer
*/
uint8_t SoundBuzzer::GetBuzzerState()
{
	return buzzer_state;
}

/*! -------------------------------------------------------------------------
\brief Play track Ode to Joy
\details   This function set TIM PWM with selected note frequency and TIM 
with note duration. TIM PWM frequency MUST be set with 4 MHz input clock.
*/
void SoundBuzzer::PlayMusic(const uint8_t *music, const uint8_t *duration, uint8_t tempo, const uint8_t size)
{
	current_music = music;
	current_duration = duration;
	play_note_time = 600000/tempo;					// 60 sec * 1000 = 60000 ms, and *10 because of TIM prescaler
	silent_note_time = play_note_time/10;
	current_play_size = size;
	buzzer_state = BUZZER_PLAY;
	note_counter = 0;
	PlayNote();
}

void SoundBuzzer::ClearValues()
{
	fading = 127;				
	A = 127;
}

/*! -------------------------------------------------------------------------
\brief Play note of the music song
\details Checks note duration and state of the buzzer. Sets timers according
to calculated values.
\param[out] 0 - all fine, 1 - error
*/
bool SoundBuzzer::PlayNote()
{
	if((DMA1_Channel2->CCR & DMA_CCR_EN) == 0){
		Phy.StartDMACh2(selected_note_dotes);
		Phy.UpdateTIM16(1);
	}
	
	if(A != 0) {
		for(uint16_t n = 0; n < selected_note_dotes; n++) {
				sound_sin[n] = (sin_value[step * n] * A)/100 + 127;
			}
			A--;
	}
	else if(fading != 255) {
		for(uint16_t n = 0; n < selected_note_dotes; n++) {
			sound_sin[n] = fading;
		}
		fading++;
	}
	else {
		if(A == 0) {
		fading = 127;				
		A = 127;
		Phy.StopDMACh2();
		Phy.StopTIM16();
				
		}
	}
	
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Set frequency and duration timers to play note
\details   
*/
void SoundBuzzer::SetTimers(uint16_t note_frequency, uint16_t note_duration)
{
	if(note_frequency == 0)
	{
		Phy.StopTimPWM();
	}
	else
	{
		Phy.UpdateTimPWM(note_frequency, note_frequency/2);
	}
	
	Phy.UpdateTIM16(note_duration);
}

/*! -------------------------------------------------------------------------
\brief Set frequency and duration timers to play note
\details   
*/
void SoundBuzzer::StopMusic()
{
	Phy.StopTimPWM();
	Phy.StopTIM16();
	buzzer_state = BUZZER_IDLE;
}
