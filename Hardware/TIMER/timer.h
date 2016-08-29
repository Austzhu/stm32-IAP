#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "adc.h"
#include "Config.h"
#include "Format_Data.h"


extern u16 times;

void TIM3_Int_Init(u16 arr,u16 psc);
 
#endif
