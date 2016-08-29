#ifndef __Tim_h__
#define __Tim_h__
#include "stm32f10x.h"
#define BASE_VAL 	40000
#define CCR1_Val 	40000	//4S
#define CCR2_Val 	30000	//3S
#define CCR3_Val 	20000	//2S
#define CCR4_Val 	10000	//1s

#define CONFIG_TIM2

extern void Tim_Init(void);

#endif
