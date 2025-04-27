#include "periphery.h" 

// Temperature sensor calibration value address 

// Temperature sensor calibration value address 30 celsius degree
// and 12 bit ADC resolution, Vpower = 3.3 V

#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFF75A8))
#define REF_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFF75AA))

/*! -------------------------------------------------------------------------
\brief Get data from temperature sensor
*/
int32_t Periphery::GetTemperature()
{
	int32_t temperature_C;
	uint8_t adc_value = 0;
	
	uint32_t temperature_adc = GetAdcTemperature();		// Get temperature in ADC	
	temperature_C = ComputeTemperature(temperature_adc);
	
	return temperature_C;
}

/*! -------------------------------------------------------------------------
\brief Calculate temperature from ADC value
\details Coefficients precalculated on PC from ADC 30 celsius degree calibration
value and slop 10 mV/C
ADC_value = k * C + b =>
C = (ADC - b) / k
b = 666
k = 12.4 adc/C
*/
int32_t Periphery::ComputeTemperature(uint32_t temperature_adc)
{
	int32_t temperature_C = temperature_adc * 10 - 6660;
	temperature_C /= 124;
	
	return(temperature_C);
}


