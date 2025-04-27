#ifndef LED_SENSOR_H
#define LED_SENSOR_H

#include "main.h" 

const uint8_t ILLUMINATION_ARRAY_SIZE = 10;

class LedSensorClass
{
	public:
		void LedOn();
		void LedOff();
		bool CheckIllumination(uint8_t &illumination_percent);
	
	private:
		void LedEnableSensor();
		uint32_t counter;
		uint32_t counter_array[ILLUMINATION_ARRAY_SIZE];
		uint8_t array_index;
};

extern LedSensorClass LedSensor;

#endif
