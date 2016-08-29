#ifndef __Init_h__
#define __Init_h__

#include "Include.h"
#include "Format_Data.h"

#define Single_open  {GPIOA->BRR		= 0x01<<3;}
#define Single_close {GPIOA->BSRR		= 0x01<<3;}

extern void Haedware_Init(void);	//硬件初始化
extern void SoftReset(void);		//软件复位

#endif //#ifndef __Init_h__

