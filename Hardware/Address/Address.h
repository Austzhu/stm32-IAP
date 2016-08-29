#ifndef __ADDRE_H__
#define __ADDRE_H__		 
#include "sys.h"	

#define Pout(GPIOx,Pin,data)	(GPIOx->BSRR = (0x01)<<Pin*((!!data) +1) )

#define PL1_L  (GPIOB->BRR  = 0x01<<4)
#define PL1_H  (GPIOB->BSRR = 0x01<<4)
#define CLK1_L (GPIOB->BRR  = 0x01<<6)
#define CLK1_H (GPIOB->BSRR = 0x01<<6)

#define PL2_L  (GPIOB->BRR  = 0x01<<9)
#define PL2_H  (GPIOB->BSRR = 0x01<<9)
#define CLK2_L (GPIOB->BRR  = 0x01<<7)
#define CLK2_H (GPIOB->BSRR = 0x01<<7)


//#define PL1  PBout(4)
//#define CLK1 PBout(6)
//#define PL2  PBout(9)
//#define CLK2 PBout(7)

#define Data1 (!!((GPIOB->IDR)&(0X01<<5)))
#define Data2 (!!((GPIOB->IDR)&(0X01<<8)))


//#define Data1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
//#define Data2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)

extern void ADDR_Init(void);
extern void Get_Address(vu8 *Addr, u8*Baud);
#endif
