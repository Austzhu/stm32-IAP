#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
#include "Interface.h"

#define CS_VRMS  0
#define CS_IRMS  1
#define N        64

void Adc_Init(void);
float  Get_Adc(u8 ch); 
float Get_Adc_Average(u8 ch,u8 times); 
u16 filter(u8 CS);
 
#endif 
