#include "periphery.h"

#define DEBUG

/*! -------------------------------------------------------------------------
\brief GPIO initialization for active mode
\details  
	MODER settings 00: input mode,	01: output mode, 10: alternate function mode, 11: analog mode
	PUPDR settings: 00 - no pull-up, no pull-down; 01 - pull-up; 10 - pull-down; 11 - reserve 
*/
void Periphery::InitGPIO()
{
	// Tick for GPIO
	RCC->IOPENR |= ( RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN);							// IO port A clock enabled
	RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
	
	// ----------------- PORT A ----------------
	// PA0 - MINUTES
	GPIOA->MODER &= ~(GPIO_MODER_MODE0_1 | GPIO_MODER_MODE0_0);	// 00: input mode
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_1;											
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0_0;													// 10: pull-down
		
	// PA1 - NC
#ifndef DEBUG 
	GPIOA->MODER |= (GPIO_MODER_MODE1_1 | GPIO_MODER_MODE1_0);		// 11: analog mode	
#else 	// VIRTUAL GND
	GPIOA->MODER |= GPIO_MODER_MODE1_0;														// 01: output mode
	GPIOA->MODER &= ~GPIO_MODER_MODE1_1;							
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT1;														// push-pull mode	
	GPIOA->BSRR = GPIO_BSRR_BR1;
#endif

	// PA2 - NC
#ifndef DEBUG
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE2_0);		// 11: analog mode
#else 	// VIRTUAL POWER 3V3
	GPIOA->MODER |= GPIO_MODER_MODE2_0;														// 01: output mode
	GPIOA->MODER &= ~GPIO_MODER_MODE2_1;							
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT2;														// push-pull mode	
	GPIOA->BSRR = GPIO_BSRR_BS2;
#endif
	
	// PA3 - ILLUMINATION SENSOR
	GPIOA->MODER |= (GPIO_MODER_MODE3_1 | GPIO_MODER_MODE3_0);		// 11: analog mode	
	
	// PA4 - nOE 
	GPIOA->MODER |= GPIO_MODER_MODE4_1;							
	GPIOA->MODER &= ~GPIO_MODER_MODE4_0;													// 10: alternate function mode
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4_1;											// 11: very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4_0;	
	GPIOA->AFR[0] |= (0x04 << 4*4);																// PA4 has AF4 alternate function (n_AF << N_PIN * 4)
	
	// PA5 - SPI_SCK
	GPIOA->MODER |= GPIO_MODER_MODE5_1;														// 10: alternate function mode AF0
	GPIOA->MODER &= ~GPIO_MODER_MODE5_0;		
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;											// 11: very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_0;		
	
	// PA6 - ST			
	GPIOA->MODER |= GPIO_MODER_MODE6_0;														// 01: output mode
	GPIOA->MODER &= ~GPIO_MODER_MODE6_1;							
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;														// push-pull mode	
	REG_ST_RESET;

  // PA7 - SPI1_MOSI
	GPIOA->MODER |= GPIO_MODER_MODE7_1;														// 10: alternate function mode AF0
	GPIOA->MODER &= ~GPIO_MODER_MODE7_0;									
	//	GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;												// push-pull mode	
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_1;											// 11: very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_0;	

	// PA11 [PA9] - NC

	// PA12 [PA10] - HOURS
	// PA10
	GPIOA->MODER &= ~(GPIO_MODER_MODE12_1 | GPIO_MODER_MODE12_0);	// 00: input mode											
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD12_1;											
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD12_0;													// 10: pull-down
	
	// ----------------- PORT B ----------------
	// PB0 [PB1] [BP2] [PA8] - NC
	#ifdef DEBUG
	// PA8 - MCO
	GPIOA->MODER |= GPIO_MODER_MODE8_1;							
	GPIOA->MODER &= ~GPIO_MODER_MODE8_0;														// 10: alternate function mode
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;												// 11: very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_0;	
	
	// MCO outpu on PA8
	RCC->CFGR |= RCC_CFGR_MCOSEL_0;
	#endif
	
	// PB3 [PB4] [PB5] [PB6] - SOUND
	// PB3
	GPIOB->MODER |= GPIO_MODER_MODE3_1;							
	GPIOB->MODER &= ~GPIO_MODER_MODE3_0;													// 10: alternate function mode
	GPIOB->AFR[0] |= (0x01 << 3*4);																// PB3 has AF1 alternate function (n_AF << N_PIN * 4)
		
	// PB4 - NC
	GPIOB->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE4_0);

	// PB5 - NC
	GPIOB->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE5_0);
	
	//PB6 - NC
	GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE6_0);

	// PB7 [PB8] - I2C SCL
	// PB7
	GPIOB->MODER |= GPIO_MODER_MODE7_1 | GPIO_MODER_MODE7_0;			// 11: analog mode

	// PB8
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_I2C_PB8_FMP;
	GPIOB->MODER |= GPIO_MODER_MODE8_1;							
	GPIOB->MODER &= ~GPIO_MODER_MODE8_0;													// 10: alternate function mode
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;											// 11: very high speed
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_0;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_1; 
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_2;
	GPIOB->OTYPER |= GPIO_OTYPER_OT8;															// 0: push-pull mode
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD8_1;											
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;														// 01: pull-up
	
	// PC14 [PB9] - I2C_SDA
	GPIOC->MODER |= (GPIO_MODER_MODE14_1 | GPIO_MODER_MODE14_0);	// 11: analog mode

	// PB9
	GPIOB->MODER |= GPIO_MODER_MODE9_1;							
	GPIOB->MODER &= ~GPIO_MODER_MODE9_0;													// 10: alternate function mode
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2); 
	GPIOB->OTYPER |= GPIO_OTYPER_OT9;															// 0: push-pull mode
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD9_1;											
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD9_0;														// 01: pull-up
	
	// ----------------- PORT C ----------------
	// PC15 - NC
	GPIOC->MODER |= (GPIO_MODER_MODE15_1 | GPIO_MODER_MODE15_0);	// 11: analog mode
}
