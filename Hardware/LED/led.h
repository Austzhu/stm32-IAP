#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_ON  {GPIOB->BRR		= 0x01<<1;}
#define LED_OFF {GPIOB->BSRR	= 0x01<<1;}

#define ledon  {GPIOB->BRR 	= 0x01<<1; delay_ms(500);}
#define ledoff {GPIOB->BSRR = 0x01<<1;	delay_ms(500);}
#define led_glint {ledon ledoff}

#define LED0  PBout(12)
void LED_Init(void);
	 				    
#endif
