#ifndef __US_TIMING_H
#define __US_TIMING_H

#include <stdint.h>
#include "tim.h"


//#define USE_HAL_DELAY_AND_ASM 1
//#define USE_DWT_TIMING 1
#define USE_TIM11_TIMING 1

//================================================================================
//===Primitive delay==============================================================
//================================================================================

// Частота виконання циклу (4 такти і одиниця -- мегагерци, щоб зручніше на
// мікросекунди множити):
#define LOOP_FREQ (SystemCoreClock/4000000)
// LOOP_FREQ == 6 для 24МГц
// LOOP_FREQ == 16 для 64МГц
// LOOP_FREQ == 18 для 72МГц

//! Затримка в мікросекундах.
//! Увага! Не працюватиме за частот, менших за 4МГц, через значення LOOP_FREQ
//! Затримка буде трішки більшою за задану, але на лічені такти.
inline void udelay_asm (uint32_t useconds) {
	useconds *= LOOP_FREQ;

    asm volatile("   mov r0, %[useconds]    \n\t"
                 "1: subs r0, #1            \n\t"
                 "   bhi 1b                 \n\t"
                 :
                 : [useconds] "r" (useconds)
                 : "r0");
}

//================================================================================
//===DWT=based====================================================================
//================================================================================
int DWT_Init(void);

#define SystemCoreClockInMHz (SystemCoreClock/1000000)

//! Увага! Не працюватиме за частот, менших за 1МГц, через значення SystemCoreClockInMHz
inline void udelay_DWT (uint32_t useconds) {
	// DWT->CYCCNT = 0; // Максимізуємо можливий інтервал
	// Але тоді udelay_DWT i get_DWT_us не можна буде змішувати.

	useconds *= SystemCoreClockInMHz;
	while( DWT->CYCCNT < useconds){}
}

inline uint32_t get_DWT_cycles()
{
	return DWT->CYCCNT;
}

inline uint32_t get_DWT_us()
{
	return get_DWT_cycles()/SystemCoreClockInMHz;
}

inline void reset_DWT_cycles()
{
	DWT->CYCCNT = 0;
}


//================================================================================
//===TIM6=based===================================================================
//================================================================================
void TIM11_reinit();

inline uint32_t get_tim11_us()
{
	extern volatile uint32_t tim11_overflows;
	__HAL_TIM_DISABLE_IT(&htim11, TIM_IT_UPDATE);
	//__disable_irq();
	uint32_t res = tim11_overflows * 10000 + __HAL_TIM_GET_COUNTER(&htim11);
	//__enable_irq();
	__HAL_TIM_ENABLE_IT(&htim11, TIM_IT_UPDATE);
	return res;
}

inline void udelay_TIM11(uint32_t useconds) {
	uint32_t before = get_tim11_us();
	while( get_tim11_us() < before+useconds){}
}

void reset_TIM11_cycles();

inline void init_us_timing()
{
#ifdef 	USE_HAL_DELAY_AND_ASM
	return;
#elif defined USE_DWT_TIMING
	DWT_Init();
#elif defined USE_TIM11_TIMING
	TIM11_reinit();
#else
#error "Unknown timing method."
#endif
}

inline uint32_t get_us()
{
#ifdef 	USE_HAL_DELAY_AND_ASM
	return 1000*HAL_GetTick();// ДУже грубо, а що зробиш?
#elif defined USE_DWT_TIMING
	return get_DWT_us();
#elif defined USE_TIM11_TIMING
	return get_tim11_us();
#else
#error "Unknown timing method."
#endif
}

inline void udelay(uint32_t useconds)
{
#ifdef 	USE_HAL_DELAY_AND_ASM
	udelay_asm(useconds);
#elif defined USE_DWT_TIMING
	udelay_DWT(useconds);
#elif defined USE_TIM11_TIMING
	udelay_TIM11(useconds);
#else
#error "Unknown timing method."
#endif
}

inline void us_reset_counter()
{
#ifdef 	USE_HAL_DELAY_AND_ASM
// Not applicable
#elif defined USE_DWT_TIMING
	reset_DWT_cycles();
#elif defined USE_TIM11_TIMING
	reset_TIM11_cycles();
#else
#error "Unknown timing method."
#endif
}


#endif /* __US_TIMING_H */
