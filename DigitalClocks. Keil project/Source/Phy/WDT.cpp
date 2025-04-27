#include "periphery.h" 

/*! -------------------------------------------------------------------------
\brief 	Initialization of WDT

\detail Сторожевой таймер тактируется от внутреннего RC-генератора частотой в 40 кГц.
Предделителем входную частоту делим на 8, получаем 5 кГц. В регистр счета пишем
максимально возможную переменную 2^12 = 4095. Если не обновлять сторожевой таймер, 
то сброс будет происходить каждые 4095/5000 = 813 мсек. 
*/

bool Periphery::InitIWDT()
{
	IWDG->KR = ((uint32_t)0x0000CCCC);				// Enable watchdog
	IWDG->KR = ((uint32_t)0x00005555);				// Key to enable access to the IWDG_PR, IWDG_RLR, IWDG_WINR
	IWDG->PR = IWDG_PR_PR_0 | IWDG_PR_PR_1;		// Prescaler divider feeding the counter clock by 011: 32
	IWDG->RLR = IWDG_RLR_RL;									// 0FFF = 4095 = 2^12
	while (IWDG->SR); 												// Проверяем флаги
	// Обновление таймера выполняется командой IWDG->KR = ((uint32_t) 0x0000AAAA) в основном цикле
	
	return 0;
}

bool Periphery::InitWWDG()
{
	WWDG->CR |= (127 << WWDG_CR_T_Pos);
	

	WWDG->CR |= WWDG_CR_WDGA;								// Start WWDG
	return 0;
}