#include "periphery.h"

/*! -------------------------------------------------------------------------
\brief ADC initialization
\param[out] 
*/
bool Periphery::InitADC()
{
	bool function_failed = false;
	
	uint8_t calib_value = 0;
	
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_1;						// 10: HSI16 as ADC clock source
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;					// ADC1 Clock Enable
	//ADC->CCR |= ADC_CCR_LFMEN;								// Low frequency mode
	//ADC1->CFGR2 |= ADC_CFGR2_CKMODE;						// ADC1 Clock from PCLK devided by 1
	
	ADC1_COMMON->CCR |= ADC_CCR_PRESC_0;
	ADC1_COMMON->CCR |= ADC_CCR_PRESC_1;
	
	ADC1->CR |= ADC_CR_ADVREGEN;
	if ((ADC1->CR & ADC_CR_ADEN) != 0)					// ADC1 Calibration
	{
		ADC1->CR |= ADC_CR_ADDIS;
	}
	
	for(uint8_t k = 0; k < 10; k++)
	{
		// Wait till ADC is enabled
		if((ADC1->CR & ADC_CR_ADEN) == 0) { break; }
		Phy.DelayUs(100);
	}
	if((ADC1->CR & ADC_CR_ADEN) == 1) { function_failed = true; }
	
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; 					// Clear DMAEN
	
	ADC1->CR |= ADC_CR_ADCAL;									// Launch the calibration 
	
	for (uint8_t k = 0; k < 10; k++)
	{
		if ((ADC1->CR & ADC_CR_ADCAL)== 0) { break; }	
		Phy.DelayUs(100);
	}
	if ((ADC1->CR & ADC_CR_ADCAL)!= 0) 
	{ 
		function_failed = true; 
	}
		
	calib_value = (ADC1->DR & 0x7F);		// ????????????? ??????????? ???????? ?? ????? 0:6
	
	
	ADC1->CFGR1 &= ~ADC_CFGR1_CONT;						// Single conversion mode
	ADC1->CHSELR = ADC_CHSELR_CHSEL3; 				// Select chanel 3
	
//	ADC1->CFGR1 &=~ ADC_CFGR1_SCANDIR;				// forward scan from channel 0 to channel 3
	
	// Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 17.1us 
	//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; 
	
	ADC1->SMPR |= ADC_SMPR_SMP1_0;
	ADC1->SMPR &= ~ADC_SMPR_SMP1_1; 
	ADC1->SMPR |= ADC_SMPR_SMP1_2;
	
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;						
	
	ADC1->CFGR1 &= ~ADC_CFGR1_RES_0;						// Data resolution: 00 - 12 bits, 01 - 10 bits, 10 - 8 bits, 11 - 6 bits
	ADC1->CFGR1 &= ~ADC_CFGR1_RES_1;
	adc_resolution_scale = 4096;								// 2^12 = 4096
	
	// ADC1->CR |= ADC_CR_ADEN;									// Power on ADC1
	// ADC1->CR |= ADC_CR_ADSTART;							// Start of conversation
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get ADC value from temperature sensor
\details  
\param[out] ADC value
*/
uint32_t Periphery::GetAdcTemperature()
{
	uint32_t adc_value = 0;
	
	// 1. Select channel
	ADC1->CHSELR = ADC_CHSELR_CHSEL12; 				// Select chanel of temperature sensor
	
	ADC1_COMMON->CCR |= ADC_CCR_TSEN; 
	for(uint8_t n = 0; n < 200; n++) {
		asm("nop");																					// Wait for stabilization time tstart
	}
		
	// 2. Enable ADC
	ADC1->CR |= ADC_CR_ADEN; 		
	for(uint16_t n = 0; n < 1000; n++) 
	{
		if(ADC1->ISR & ADC_ISR_ADRDY) { break; }
	}
	
	// 3) Start measures
	ADC1->CR |= ADC_CR_ADSTART; 
	for(uint16_t n = 0; n < 60000; n++) {
		if(ADC1->ISR & ADC_ISR_EOC) { break; }
	}

	// 4) Read ADC
	adc_value = ADC1->DR; 
	
	// 5) Disable ADC
	ADC1->ISR = ADC_ISR_ADRDY;				// clear by writing 1
	ADC1->CR |= ADC_CR_ADDIS; 
	ADC1_COMMON->CCR &= ~ADC_CCR_TSEN; 
	
	ADC->CCR &= ~ADC_CCR_TSEN; 						// Send temperature sensor to sleep
	
	return adc_value;
}

/*! -------------------------------------------------------------------------
\brief Get ADC value from reference voltage 1.23V
\details  
\param[out] ADC value
*/
uint32_t Periphery::GetAdcReference()
{
	uint32_t adc_value = 0;
	
	// 1. Select channel
	ADC1->CHSELR = ADC_CHSELR_CHSEL13; 				// Select chanel 13
	ADC1_COMMON->CCR |= ADC_CCR_VREFEN; 
	
	// 2. Enable ADC
	ADC1->CR |= ADC_CR_ADEN; 		
	for(uint16_t n = 0; n < 1000; n++) 
	{
		if(ADC1->ISR & ADC_ISR_ADRDY) { 
			break; 
		}
	}
	
	// 3) Start measures
	ADC1->CR |= ADC_CR_ADSTART; 
	for(uint16_t n = 0; n < 60000; n++) {
		if(ADC1->ISR & ADC_ISR_EOC) 
		{ 
			break; 
		}
	}

	// 4) Read ADC
	adc_value = ADC1->DR; 
	
	// 5) Disable ADC
	ADC1->ISR = ADC_ISR_ADRDY;				// clear by writing 1
	ADC1->CR |= ADC_CR_ADDIS; 
	ADC1_COMMON->CCR &= ~ADC_CCR_VREFEN;
	
	return adc_value;
}

/*! -------------------------------------------------------------------------
\brief Get ADC value from battery channel
\details  
\param[out] ADC value
*/
bool Periphery::GetIlluminationAdcValue(uint16_t &illumination_adc)
{
	bool function_failed1 = true;
	bool function_failed2 = true;
	
	// 1. Select channel
	ADC1->CHSELR = ADC_CHSELR_CHSEL3; 				// Select chanel 
	
	// 2. Enable ADC
	ADC1->CR |= ADC_CR_ADEN; 		
	for(uint16_t n = 0; n < 1000; n++) {
		if(ADC1->ISR & ADC_ISR_ADRDY) { 
			function_failed1 = false;
			break; 
		}
	}
	
	// 3) Start measures
	ADC1->CR |= ADC_CR_ADSTART; 
	for(uint16_t n = 0; n < 60000; n++) {
		if(ADC1->ISR & ADC_ISR_EOC) { 
			function_failed2 = false;
			break; 
		}
	}

	// 4) Read ADC
	illumination_adc = ADC1->DR; 
	
	// 5) Disable ADC
	ADC1->ISR = ADC_ISR_ADRDY;				// clear by writing 1
	ADC1->CR |= ADC_CR_ADDIS; 
	
	return function_failed1 | function_failed2;
}
