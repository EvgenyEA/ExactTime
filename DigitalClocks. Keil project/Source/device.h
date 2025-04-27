#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

const uint32_t BIG_PERIOD_TIME = 0xFFFF0000;            // not max, during tick increment we can overlap max
const uint32_t MAX_SET_TIME_TIMEOUT_SEC = 15;

typedef enum illumination_level {
	ILLUMINATION_UNDEFINED = 0,
	ILLUMINATION_MAX = 1,
	ILLUMINATION_MIN = 2
} illumination_level_t;

class DeviceClass
{
	public:
		bool InitDevice();
		bool CheckWaitingEvent();
		
		void IncrementGlobalTime();
		uint32_t GetGlobalTime();	
		bool StartEventNow(const uint8_t event_number);
		
	protected:
		bool EventButtonMinutes();	uint32_t event_button_minutes_time;			// 1
		bool EventButtonHours();		uint32_t event_button_hours_time;				// 2
		bool EventBuzzer();					uint32_t event_buzzer_time;					// 
		bool EventLed();						uint32_t event_led_time;						// 
																uint8_t charge_counter;
		bool EventPowerVoltage();		uint32_t event_power_voltage_time;	// 														
		bool EventTimer();					uint32_t event_timer_time;					// 
		bool EventWdt();																								// 
		bool EventFlash();					uint32_t event_flash_time;					//
		bool EventRtc();						uint32_t event_rtc_time;		
		
		bool CheckEventTime(uint32_t &time);
		uint32_t period_of_event;
		
	private:
		volatile uint32_t global_time;
		bool TuneBrightness();
		bool IncrementHours();
		bool IncrementMinutes();
	
		static const uint8_t MIN_BRIGHTNESS_INDEX = 9;
		static const uint8_t MAX_BRIGHTNESS_INDEX = 19;
		bool brightness_up; 
		uint8_t brightness_index; 
		uint8_t illumination_adc_index;
		illumination_level_t illumination_level;
		uint32_t set_time_timeout_sec; 
		uint8_t sount_times;
	
			// Parameters
		bool WriteParamToFlah();
};

extern DeviceClass Device;


#endif
