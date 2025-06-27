#include "device.h" 
#include "periphery.h" 
#include "event_manager.h" 
#include "button.h"
#include "buzzer.h" 
#include "notes.h" 
#include "parameters.h" 
#include "DS3231.h" 
#include "display_segment.h"
#include "led_sensor.h" 
#include "fsm.h" 

bool button_long = false;

const uint8_t BRIGHTNESS[] = 
{
	0, 		0, 		0, 		2, 		4, 		7, 		10, 	15, 	22, 	30,
	40, 	52, 	66, 	83, 	102, 	124, 	148, 	176, 	207, 	255
};


/*! -------------------------------------------------------------------------
\brief Urgent start of event
\details  
*/
bool DeviceClass::StartEventNow(const uint8_t event_number)
{
  bool function_failed = false;
	
  switch(event_number)
  {	
    case EVENT_RTC:							event_rtc_time           	= BIG_PERIOD_TIME;	break;
    case EVENT_LED:							event_led_time            = BIG_PERIOD_TIME; 	break;
    case EVENT_POWER_VOLTAGE:   event_power_voltage_time  = BIG_PERIOD_TIME; 	break;
    default: 										function_failed = true;												break;
  }
return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Increment global time value
\details  
*/
void DeviceClass::IncrementGlobalTime()
{
	global_time++;
	if(Events.CheckIfActive(EVENT_BUTTON_MINUTES))	{ event_button_minutes_time++; }
	if(Events.CheckIfActive(EVENT_BUTTON_HOURS))		{ event_button_hours_time++; }
	if(Events.CheckIfActive(EVENT_BUZZER))					{ event_buzzer_time++; }
	if(Events.CheckIfActive(EVENT_LED))							{ event_led_time++; }
	if(Events.CheckIfActive(EVENT_TIMER))						{ event_timer_time++; }
	if(Events.CheckIfActive(EVENT_FLASH))						{ event_flash_time++; }
	if(Events.CheckIfActive(EVENT_POWER_VOLTAGE)) 	{ event_power_voltage_time++; }
	if(Events.CheckIfActive(EVENT_RTC))							{ event_rtc_time++;	}
}	

/*! -------------------------------------------------------------------------
\brief Check event time schedule
\param[out] 0 - excecute event
						1 - wait 
*/
bool DeviceClass::CheckEventTime(uint32_t &time)
{
	if(time >= period_of_event)
	{
		time = 0;
		return 0;
	}		
	return 1;
}

/*! -------------------------------------------------------------------------
\brief Function check waiting event
\details We MUST FULFULL swithc structure by FULL number of events 
*/
bool DeviceClass::CheckWaitingEvent()
{
	bool function_failed = true;
	uint8_t event_number = EVENT_NONE;
	uint32_t event_period = 0;
	do 
	{
		Events.GetWaitingEvent(event_number, event_period);
		period_of_event = event_period;
		
		switch(event_number)
		{	
			case EVENT_BUTTON_MINUTES:	  function_failed = EventButtonMinutes();		break;		// 1
			case EVENT_BUTTON_HOURS:	  	function_failed = EventButtonHours();			break;		// 
			case EVENT_BUZZER:						function_failed = EventBuzzer();					break;		// 
			case EVENT_LED:								function_failed = EventLed();							break;		// 
			case EVENT_POWER_VOLTAGE:			function_failed = EventPowerVoltage();		break;		// 
			case EVENT_TIMER:							function_failed = EventTimer();						break;		// 
			case EVENT_WDT:								function_failed = EventWdt();							break;		// 
			case EVENT_FLASH:							function_failed = EventFlash();						break;		//
			case EVENT_RTC:								function_failed = EventRtc();							break;		
			
			default: 
			{
				Events.ClearExecutedEvents();
				break;
			}
		}
	}
	while(event_number != EVENT_NONE);
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_BUTTON_HOURS (2)
\details  
*/
bool DeviceClass::EventButtonHours()
{
	if(!CheckEventTime(event_button_hours_time))
	{
		states_t current_state = Fsm.GetCurrentState();
		Display.ClearBlinkCounter();
		uint8_t status = Buttons.Check();
			
		switch(status)
		{
			case BUTTON_WAIT:
			{
				if(current_state == STATE_TIME) {
					if(BUTTON_HOURS_STATE) {
						set_time_timeout_sec = 0;
						IncrementHours();
						Buttons.ButtonHoursPressed();
						Events.AddEvent(EVENT_BUTTON_HOURS, 350);
					}
				}
				else if(current_state == STATE_BRIGHTNESS) {
					if(BUTTON_HOURS_STATE) {
						set_time_timeout_sec = 0;
						brightness_up = true;
						TuneBrightness();
						Events.AddEvent(EVENT_BUTTON_HOURS, 650);
					}
				}
			}
			break;
			
			case BUTTON_HOURS_SHORT: {
				set_time_timeout_sec = 0;
				if(current_state == STATE_TIME) {				
					uint8_t current_hours = IrParamWords.ds3231_hours;
					if(!button_long) {
						current_hours++;
						if(current_hours > 23) {
							current_hours = 0;
						}
						RTC_DS3231.setHour(current_hours);
						Device.StartEventNow(EVENT_RTC);
							
						Events.AddEvent(EVENT_BUZZER, 1);
					}
					else {
						asm("nop");
					}
						
					button_long = false;
					Events.ClearEvent(EVENT_BUTTON_HOURS);
				}
				else if(current_state == STATE_BRIGHTNESS) {
					Events.AddEvent(EVENT_BUZZER, 1);
					brightness_up = true;
					TuneBrightness();
				}	
			}
			break;
				
			case BUTTON_HOURS_HALF_LONG: {
				if(current_state == STATE_WORK) {
					Events.AddEvent(EVENT_BUZZER, 1);
				}
			}
			break;
				
			case BUTTON_HOURS_LONG: {
				if(current_state == STATE_WORK) {
					sount_times = 2;
					Events.AddEvent(EVENT_BUZZER, 1);
					Fsm.ChangeState(STATE_TIME);
					Events.ClearEvent(EVENT_BUTTON_HOURS);
				}
			}
			break;
				
			case BUTTONS_1_AND_2_LONG: {
				Events.AddEvent(EVENT_BUZZER, 1);
				Events.ClearEvent(EVENT_BUTTON_MINUTES);
				Fsm.ChangeState(STATE_BRIGHTNESS);
			}
				
			default:
				Events.ClearEvent(EVENT_BUTTON_HOURS);
				button_long = false;
				break;
		}
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Increment hours
\details  
*/
bool DeviceClass::IncrementHours() 
{
	uint8_t current_hours = IrParamWords.ds3231_hours;
	current_hours++;
	if(current_hours > 23) {
		current_hours = 0;
	}
	RTC_DS3231.setHour(current_hours);
	Device.StartEventNow(EVENT_RTC);

	if(button_long == false) {
		Events.AddEvent(EVENT_BUZZER, 1);
	}
	
	button_long = true;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_BUTTON_MINUTES (1)
\details  
*/
bool DeviceClass::EventButtonMinutes()
{	
	if(!CheckEventTime(event_button_minutes_time))
	{
		states_t current_state = Fsm.GetCurrentState();
		Display.ClearBlinkCounter();
		uint8_t status = Buttons.Check();
		switch(status)
		{
			case BUTTON_WAIT:
			{
				if(current_state == STATE_TIME) {
					if(BUTTON_MINUTES_STATE) {
						set_time_timeout_sec = 0;
						IncrementMinutes();
						Buttons.ButtonMinutesPressed();
						Events.AddEvent(EVENT_BUTTON_MINUTES, 260);
					}
				}
				else if(current_state == STATE_BRIGHTNESS) {
					if(BUTTON_MINUTES_STATE) {
						set_time_timeout_sec = 0;
						brightness_up = false;
						TuneBrightness();
						Events.AddEvent(EVENT_BUTTON_MINUTES, 650);
					}
				}
			}
			break;
			
			case BUTTON_MINUTES_SHORT: 
			{
				set_time_timeout_sec = 0;
				if(current_state == STATE_TIME) {				
					uint8_t current_minutes = IrParamWords.ds3231_minutes;
					if(!button_long) {
						current_minutes++;
							
						if(current_minutes > 59) {
							current_minutes = 0;
						}
						RTC_DS3231.setSecond(0);
						RTC_DS3231.setMinute(current_minutes);
						Device.StartEventNow(EVENT_RTC);
							
						Events.AddEvent(EVENT_BUZZER, 1);
					}
					else {
						asm("nop");
					}
					button_long = false;
					Events.ClearEvent(EVENT_BUTTON_MINUTES);
				}
				else if(current_state == STATE_BRIGHTNESS) {
					Events.AddEvent(EVENT_BUZZER, 1);
					brightness_up = false;
					TuneBrightness();
				}
			}
			break;
				
			case BUTTON_MINUTES_HALF_LONG: 	{
				if(current_state == STATE_WORK) {
					Events.AddEvent(EVENT_BUZZER, 1);
				}			
			}
			break;
			
			case BUTTON_MINUTES_LONG: {
				if(current_state == STATE_WORK) {
					sount_times = 2;
					Events.AddEvent(EVENT_BUZZER, 1);
					Fsm.ChangeState(STATE_TIME);
					Events.ClearEvent(EVENT_BUTTON_MINUTES);
				}
			}
			break;	
				
			case BUTTONS_1_AND_2_LONG: {
				Events.AddEvent(EVENT_BUZZER, 1);
				Events.ClearEvent(EVENT_BUTTON_MINUTES);
				Fsm.ChangeState(STATE_BRIGHTNESS);
			}
				
			default:
				Events.ClearEvent(EVENT_BUTTON_MINUTES);
				button_long = false;
				break;
		}
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Increment minutes
\details  
*/
bool DeviceClass::IncrementMinutes()
{
	uint8_t current_minutes = IrParamWords.ds3231_minutes;
	current_minutes++;
	if(current_minutes > 59) {
		current_minutes = 0;
	}
	RTC_DS3231.setSecond(0);
	RTC_DS3231.setMinute(current_minutes);
	Device.StartEventNow(EVENT_RTC);
	if(button_long == false) {
		Events.AddEvent(EVENT_BUZZER, 1);
	}

	Buttons.ButtonMinutesPressed();
	button_long = true;
}

/*! -------------------------------------------------------------------------
\brief Tune the brightness of 7 segment dispaly
\details  
*/
bool DeviceClass::TuneBrightness() {
	bool tune_finised = false;
	if(brightness_up) {
		brightness_index < MAX_BRIGHTNESS_INDEX ? brightness_index++ : tune_finised = true;
	}
	else {
		brightness_index > MIN_BRIGHTNESS_INDEX ? brightness_index-- : tune_finised = true;
	}
	

	
	IrParamWords.brightness = BRIGHTNESS[brightness_index];
	Phy.SetDisplayBrightness(BRIGHTNESS[brightness_index]);
	
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_LED (4)
\details  
*/
bool DeviceClass::EventLed()
{
	if(!CheckEventTime(event_led_time)) {
		uint16_t illumination_adc = 0;
		// 1. Get illumination_value
		if(!Phy.GetIlluminationAdcValue(illumination_adc)) {
			float illumination_voltage = (illumination_adc * 3300.0) / 4096.0;
			IrParamWords.illumination[illumination_adc_index] = uint16_t(illumination_voltage);
			illumination_adc_index++;
			if(illumination_adc_index > ILLUM_ARRAY_SIZE) {
				illumination_adc_index = 0;
			}
			
			// 2. Calculate average
			IrParamWords.illumination_average = IrParamWords.illumination[0];
			for(uint8_t n = 1; n < ILLUM_ARRAY_SIZE; n++) {
				if(IrParamWords.illumination[n] > 0) {				
					IrParamWords.illumination_average += IrParamWords.illumination[n];
					IrParamWords.illumination_average /= 2;
				}
				else {
					break;
				}
			}
			
			// 3. Tune brightness
			if(IrParamWords.illumination_average > (ILLUMINATION_THRESHOLD + ILLUMINATION_HISTERESIS) && illumination_level != ILLUMINATION_MAX) {
				Phy.SetDisplayBrightness(IrParamWords.brightness);
				illumination_level = ILLUMINATION_MAX;
			}
			else if(IrParamWords.illumination_average < (ILLUMINATION_THRESHOLD - ILLUMINATION_HISTERESIS) && illumination_level != ILLUMINATION_MIN) {
				Phy.SetDisplayBrightness(BRIGHTNESS[MIN_BRIGHTNESS_INDEX]);
				illumination_level = ILLUMINATION_MIN;
			}
		}
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_POWER_VOLTAGE (6)
\details  
*/
bool DeviceClass::EventPowerVoltage()
{
	if(!CheckEventTime(event_power_voltage_time))
	{
//		Phy.GetPowerVoltage(ParamWords.power_voltage);
//		if(ParamWords.power_voltage < 9000) {
//			
//			ParamWords.error_type = ERROR_LOW_POWER_VOLTAGE;
//		}
//		else if(!pump_is_active) {
//			ParamWords.error_type = ERROR_NONE;
//		}
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_PUMP (7)
\details  
*/

bool DeviceClass::EventWdt()
{
	IWDG->KR = ((uint32_t) 0x0000AAAA);
	return 0;
}


/*! -------------------------------------------------------------------------
\brief Handle EVENT_BUZZER (3)
\details  
*/
bool DeviceClass::EventBuzzer()
{
	if(!CheckEventTime(event_buzzer_time)) {	
		Buzzer.ClearValues();
		Buzzer.PlayNote();
		
		if(sount_times > 1) {
			sount_times--;
			Events.AddEvent(EVENT_BUZZER, 400);
		}
		else {
			sount_times = 1;
			Events.ClearEvent(EVENT_BUZZER);
		}
	}
	return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_SLEEP (10)
\details  
*/
bool DeviceClass::EventTimer()
{
 if(!CheckEventTime(event_timer_time)) {
	 states_t current_state = Fsm.GetCurrentState();
	 if((current_state == STATE_TIME) || (current_state == STATE_BRIGHTNESS)) {
		 set_time_timeout_sec++;
		 if(set_time_timeout_sec > MAX_SET_TIME_TIMEOUT_SEC) {
			 Fsm.ChangeState(STATE_WORK);
			 set_time_timeout_sec = 0;
			 Events.AddEvent(EVENT_FLASH, 100);
		 }
	 }
	 
 }
  return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_RTC
\details  
*/
bool DeviceClass::EventRtc()
{
 if(!CheckEventTime(event_rtc_time)) {
	 
	 bool oscillator_faile = false;
	 if(!RTC_DS3231.oscillatorCheck(oscillator_faile))
	 {
		 if(oscillator_faile) {					// received data isn't valid
			 RTC_DS3231.setSecond(0);
			 RTC_DS3231.setMinute(0);
			 RTC_DS3231.setHour(0);
		 }
	 }
	 
//	 float temperature = 0;
//	 if(!RTC_DS3231.getTemperature(temperature)) {
//		 IrParamWords.ds3231_temperature = temperature * 100;
//		}
	 
	uint8_t seconds = 0;
	 if(!RTC_DS3231.getSeconds(seconds)) {
		 IrParamWords.ds3231_seconds = seconds;
		}
	
		uint8_t minutes = 0;
		if(!RTC_DS3231.getMinutes(minutes)) {
		 IrParamWords.ds3231_minutes = minutes;
		}
		
		uint8_t hours = 0;
		bool h12 = false;
		bool pm_time = false;
		if(!RTC_DS3231.getHours(hours, h12, pm_time)) {
		 IrParamWords.ds3231_hours = hours;
		}
		
//		uint8_t DoW = 0;
//		if(!RTC_DS3231.getDoW(DoW)) {
//		 IrParamWords.ds3231_day = DoW;
//		}
//		
//		uint8_t date = 0;
//		if(!RTC_DS3231.getDate(date)) {
//		 IrParamWords.ds3231_date = date;
//		}
//		
//		uint8_t month = 0;
//		bool century = false;
//		if(!RTC_DS3231.getMonth(month, century)){
//			IrParamWords.ds3231_month = month;
//		}
//		
//		uint8_t year = 0;
//		if(!RTC_DS3231.getYear(year)) {
//		 IrParamWords.ds3231_years = year;
//		}

	}
  return 0;
}

/*! -------------------------------------------------------------------------
\brief Handle EVENT_FLASH (11)
\details  
*/
bool DeviceClass::EventFlash()
{
	if(!CheckEventTime(event_flash_time)) 
	{
		bool write_success = true;
		uint8_t data_write[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		uint8_t data_check[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		
		Phy.FlashErasePage(15);
		
		data_write[0] = IrParamWords.brightness;
		data_write[7] = 0xEA; 																// write end delimeter
		Phy.FlashWrite(FLASH_PARAM_ADDRESS, data_write, 8);
		Phy.FlashRead(FLASH_PARAM_ADDRESS, data_check, 8);
		
		for(uint8_t n = 0; n < 8; n++) {
			if(data_check[n] != data_write[n]) {
					write_success = false;
			}
		}
		
		if(write_success) {
			asm("nop");
		}
		
		Events.ClearEvent(EVENT_FLASH);
 }
  return 0;
}
