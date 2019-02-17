#include "stm32f4xx_hal.h"

#include "us_timing.h"


//================================================================================
//===DWT=based====================================================================
//================================================================================
// Код -- за мотивами stm32f4-discovery.net/2015/07/all-stm32-hal-libraries/
// ДИв. також -- можливе обмеження на розмір цього регістру: https://github.com/spark/firmware/issues/406
int DWT_Init(void) {

	/* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Enable counter */
    DWT->CTRL |=  SysTick_CTRL_ENABLE_Msk;

    /* Reset counter */
    DWT->CYCCNT = 0; // Для STM32F3 -- 32-бітний

	/* Check if DWT has started */
	uint32_t before = DWT->CYCCNT;
	__NOP();
	__NOP();

	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - before);
}

//================================================================================
//===TIM6=based===================================================================
//================================================================================

volatile uint32_t tim11_overflows = 0;
extern volatile uint16_t Timer1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 if( htim->Instance == TIM11 )
	 {
		 ++tim11_overflows;
	 }
	 if(htim==&htim2){
	 		Timer1++;
	 	}
}


void TIM11_reinit()
{
	HAL_TIM_Base_Stop(&htim11);
	__HAL_TIM_SET_PRESCALER( (&htim11), (SystemCoreClockInMHz-1) );
	__HAL_TIM_SET_COUNTER( &htim11, 0 );
	tim11_overflows = 0;
	HAL_TIM_Base_Start(&htim11);
}

void reset_TIM11_cycles()
{
	tim11_overflows = 0;
	__HAL_TIM_SET_COUNTER( &htim11, 0 );
}
