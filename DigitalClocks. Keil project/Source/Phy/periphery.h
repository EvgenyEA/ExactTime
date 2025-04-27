#ifndef PERIPHERY_H
#define PERIPHERY_H

#include <stdint.h>
#include "stm32g030xx.h"

// PA0 - BUTTON_MINUTES
#define BUTTON_MINUTES_STATE 				(GPIOA->IDR & GPIO_IDR_ID0)					// High level is active

// PA5 - LED/SENSORS_EN
#define LED_ENABLE									(GPIOA->BSRR = GPIO_BSRR_BS5)
#define LED_DISABLE									(GPIOA->BSRR = GPIO_BSRR_BR5)
#define LED_STATE										(GPIOA->IDR & GPIO_IDR_ID5)

// PA6 - REG_ST_SET
#define REG_ST_SET									(GPIOA->BSRR = GPIO_BSRR_BS6)
#define REG_ST_RESET								(GPIOA->BSRR = GPIO_BSRR_BR6)
#define REG_ST_STATE								(GPIOA->IDR & GPIO_IDR_ID6)

// PA12 - BUTTON HOURS
#define BUTTON_HOURS_STATE 					(GPIOA->IDR & GPIO_IDR_ID12)					// High level is active



//const uint32_t MCU_FREQUENCY = 48000000;
const uint32_t MCU_FREQUENCY = 64000000;
const uint16_t MCU_POWER_VOLTAGE_MV = 3300;
const uint16_t MCU_ADC_SCALE = 4096;
const uint16_t LSI_FREQUENCY = 32000;

// I2C
typedef enum _I2C_Direction {I2C_Transmitter = 0, I2C_Receiver = 1} I2C_Direction;

//static const uint16_t SIN_DOTS = 360;

uint8_t extern sound_sin[500];

class Periphery
{
	public:
		void InitPhy();

		// RTC
		bool RTCInterruptEnable(); 
	
		// TIM
		bool DelayUs(uint16_t);	
		bool DelayMs(uint16_t delay);
		
		// I2C1
		bool InitI2C1();
		bool WriteI2C1_DS3231(const uint8_t device_address, const uint8_t data_address, uint8_t *data, const uint8_t data_amount); 
		bool ReadI2C1_DS3231(const uint8_t device_address, const uint8_t data_address, uint8_t *data, const uint8_t data_amount);
		
		// DMA
		bool CheckDMACh1State();
		void StartDMACh1(const uint16_t data_length);	
		bool CheckDMACh2State();
		void StartDMACh2(const uint16_t data_length);
		void StopDMACh2();
		
		// Work ADC
		int32_t GetTemperature();
		uint32_t GetAdcReference();
		bool GetPowerVoltage(uint16_t &voltage);
		bool GetIlluminationAdcValue(uint16_t &illumination_adc);
		
		// GPIO
		void InitGPIO();
		
		// Work wit buzzer timers
		void UpdateTimPWM(uint16_t reload_value, uint16_t count_value);
		void StopTimPWM();
		bool UpdateTIM16(uint16_t reload_value_ms);
		void StopTIM16();
		
		// System Tick
		void StartSystemTick();
		void StopSystemTick();
		
		// Flash 
		bool FlashErasePage(uint8_t page_address);
		void FlashMassErase(void); 
		bool FlashWrite(uint32_t const addr, const uint8_t* data, uint16_t const data_len); 
    bool FlashRead(uint32_t const addr, uint8_t* data, uint16_t const data_len);
		
		// SPI1
		bool SPI1Write(const uint8_t data_send);
		bool SPI1Write(const uint8_t *data_send, const uint16_t data_lenght);
		bool SPI1Write(uint8_t data_send, uint8_t &data_receive);
		
		// TIM
		bool SetDisplayBrightness(const uint8_t brightness);
		
		void EnterStopMode();
		
	private:	
		bool InitTick();
		
		void InitInterrupts();
		void InitSPI1();	
	
		void InitSystemTimer();
		void InitTim1Pwm();
		void InitTim3Ms();
		void InitTim14DisplayBrightness();
		void InitTim16Buzzer();
		void InitTim17DisplayUpdate();

		bool InitRTC();
		void InitPowerMode();
	
		bool InitIWDT();
		bool InitWWDG();
	
		void InitDMACh1();
		void InitDMACh2();
	
		// I2C
		void ResetI2C1();
		void StartI2C1();
		bool StopI2C1();
		bool I2C_StartDirectionAddressSize (I2C_Direction Direction, uint8_t Address, uint8_t Size);
		
		static const uint8_t I2C_OFFSET_TIMINGR_SCLL = 0;
		static const uint8_t I2C_OFFSET_TIMINGR_SCLH = 8;
		static const uint8_t I2C_OFFSET_TIMINGR_SCLDEL =	20;
		static const uint8_t I2C_OFFSET_TIMINGR_PRESC = 28;
		static const uint8_t I2C_OFFSET_CR2_NBYTES = 16;
	
		// Temperature sensor
		bool InitTemperatureSensor();
		uint32_t GetAdcTemperature();
		int32_t ComputeTemperature(uint32_t measure);
		
		int32_t temperature_k_x3;
		int32_t temperature_b_x3;
		uint32_t scale_coeff_x3;

		// Flash
		bool FlashWaitBusy(); 
		bool FlashUnlock();
		bool FlashLock();

		// ADC
		bool InitADC();
		uint16_t adc_resolution_scale; 
};

extern Periphery Phy;

#endif

