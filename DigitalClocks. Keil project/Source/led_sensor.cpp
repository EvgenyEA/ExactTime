#include "led_sensor.h" 
#include "periphery.h" 

LedSensorClass LedSensor;

/*! -------------------------------------------------------------------------
\brief Turn On led
*/
void LedSensorClass::LedOn()
{
	// LED ANODE PA3
	GPIOA->BSRR = GPIO_BSRR_BS3;	
	
	// LED CATHODE PA2
	GPIOA->BSRR = GPIO_BSRR_BR2;
}

/*! -------------------------------------------------------------------------
\brief Turn Off led
*/
void LedSensorClass::LedOff()
{
	// LED ANODE PA3
	GPIOA->BSRR = GPIO_BSRR_BR3;	
	
	// LED CATHODE PA2
	GPIOA->BSRR = GPIO_BSRR_BR2;
}

/*! -------------------------------------------------------------------------
\brief Enable led in reverse direction
*/
void LedSensorClass::LedEnableSensor()
{	
	GPIOA->BSRR = GPIO_BSRR_BR2;
	for(uint32_t n = 0; n < 1000; n++) { asm("nop"); } 							// wait for internal MCU parasit capacitor to be discharged
	
	GPIOA->BSRR = GPIO_BSRR_BR3;																		// PA3 LED ANODE
	GPIOA->BSRR = GPIO_BSRR_BS2;																		// PA2 LED CATHODE
	
	for(uint32_t n = 0; n < 1000; n++) { asm("nop"); } 							// wait for internal MCU parasit capacitor to be charged
	
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0 | GPIO_MODER_MODE2_1);			// 00: LED CATHODE input mode	
}

/*! -------------------------------------------------------------------------
\brief Check Illumination 
*/
bool LedSensorClass::CheckIllumination(uint8_t &illumination_percent)
{
	bool function_failed = false;
	
	// 1. Check illumination
	LedEnableSensor();
	counter = 0;
	
	if(GPIOA->IDR & GPIO_IDR_ID2) {
		while(GPIOA->IDR & GPIO_IDR_ID2) {
			counter++;
			if(counter > 1000000) {
				function_failed = true;
				break;
			}
		}
	}
	if(counter == 0) {
		function_failed = true;
	}
	else {
		counter_array[array_index] = counter;
		array_index++;
		if(array_index == ILLUMINATION_ARRAY_SIZE) {
			array_index = 0;
		}
	}
	
	GPIOA->MODER |= GPIO_MODER_MODE2_0;							// 01: output mode
	GPIOA->MODER &= ~GPIO_MODER_MODE2_1;
//	GPIOA->BSRR = GPIO_BSRR_BR3;
	GPIOA->BSRR = GPIO_BSRR_BR2;
	
	// 2. Calculate illumination
	uint32_t counter_average = 0;
	for(uint8_t n = 0; n < ILLUMINATION_ARRAY_SIZE; n++) {
		if(counter_array[n] != 0) {
			counter_average += counter_array[n];
			counter_average /= 2;
		}
		else {
			function_failed = true;
			break;
		}
	}
	
	if(counter_average <= 100) {
		illumination_percent = 20;
	}
	else if(counter_average <= 1000) {
		illumination_percent = 40;
	}
	else if(counter_average <= 2000) {
		illumination_percent = 70;
	}
	else {
		illumination_percent = 100;
	}
	
	return function_failed;
}

