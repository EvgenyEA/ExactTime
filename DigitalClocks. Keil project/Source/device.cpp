#include "device.h" 
#include "periphery.h" 
#include "event_manager.h" 
#include "button.h" 
#include "parameters.h" 
#include "buzzer.h" 
#include "notes.h" 
#include "display_segment.h"
#include "fsm.h" 

DeviceClass Device;

/*! -------------------------------------------------------------------------
\brief Device initialization function
\details  
*/
bool DeviceClass::InitDevice()
{	
	Phy.DelayMs(300);
	Parameters.InitParameters();
	Phy.SetDisplayBrightness(IrParamWords.brightness);
	
	brightness_up = false; 
	brightness_index = MAX_BRIGHTNESS_INDEX; 
	illumination_adc_index = 0;
	illumination_level = ILLUMINATION_UNDEFINED;
	sount_times = 1;
	
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Get current global time value
\details  
*/
uint32_t DeviceClass::GetGlobalTime()
{
	return global_time;
}


/*! -------------------------------------------------------------------------
\brief Interrupts
*/
extern "C" 
{ 
/*! -------------------------------------------------------------------------
\brief System timer
*/
	void SysTick_Handler() 
	{
		Device.IncrementGlobalTime();
	}
	
/*! -------------------------------------------------------------------------
\brief GPIO interrupts 
\details Buttons MINUTES and HOURS
*/
	void EXTI0_1_IRQHandler()
	{
		if(EXTI->RPR1 & EXTI_RPR1_RPIF0) {
			Buttons.ButtonMinutesPressed();
			Events.AddEvent(EVENT_BUTTON_MINUTES, 1);
			EXTI->RPR1 = EXTI_RPR1_RPIF0;
		}
	}
	
	void EXTI4_15_IRQHandler()
	{
		if(EXTI->RPR1 & EXTI_RPR1_RPIF12) {
			Buttons.ButtonHoursPressed();
			Events.AddEvent(EVENT_BUTTON_HOURS, 1);
			EXTI->RPR1 = EXTI_RPR1_RPIF12;
		}
	}
	
/*! -------------------------------------------------------------------------
\brief DMA channel 1 Interrupt routine
*/
	void DMA1_Channel1_IRQHandler()
	{
		DMA1_Channel1->CCR  &= ~DMA_CCR_EN;
		DMA1->IFCR = DMA_IFCR_CGIF1;
		
		for(uint32_t n = 0; n < 100; n++) {
			REG_ST_SET;
			if(REG_ST_STATE) {
				break;
			}
		}
	}
	
	// Buzzer note duration interrupt
	void TIM16_IRQHandler()	{
		if(TIM16->SR & TIM_SR_UIF)
		{
			Buzzer.PlayNote();
			TIM16->SR &= ~TIM_SR_UIF;
		}
	}
	
/*! -------------------------------------------------------------------------
\brief TIM17 Interrupt routine
\detail Update display data every 1 ms
*/
	void TIM17_IRQHandler()	{
		if(TIM17->SR & TIM_SR_UIF) {
			TIM17->CR1 &= ~TIM_CR1_CEN;	
						
			for(uint32_t n = 0; n < 1000; n++) {
				REG_ST_RESET;
				if(!REG_ST_STATE) {
					break;
				}
			}
			Phy.DelayUs(100);	
			Display.UpdateDisplay();
			
			for(uint32_t n = 0; n < 100; n++) {
				REG_ST_SET;
				if(REG_ST_STATE) {
					break;
				}
			}

			TIM17->CNT = 0;
			TIM17->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
			TIM17->CR1 |= TIM_CR1_CEN;	
		}
	}
}
