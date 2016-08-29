#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define K1_L  (GPIOA->BRR = 0x01<<3)
#define K1_H  (GPIOA->BSRR = 0x01<<3)

//#define K1(x) {if(!x) K1_L;\
							else K1_H;}
extern u32 Singlelight_level;					

typedef struct
{
	u8 level;
	u8 pwm;
}Light_t;



void TIM4_PWM_Init(u16 arr,u16 psc);
void Set_light(u32 level);
//void PWM_DAC_Set(u16 vol);
#endif


