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
\brief 
\details 
*/
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
