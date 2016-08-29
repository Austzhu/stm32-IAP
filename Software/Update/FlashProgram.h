#ifndef __FlashProgram_h__
#define __FlashProgram_h__
#include "stm32f10x_flash.h"
#include "update.h" 

extern void FlashAllErase(u8 Image);
extern int  FlashProgrm(void);
extern void RCC_Config(void);
extern void Get_ImageInfo(void);
extern void Set_ImageInfo(void);
extern void Set_ImageInfo_A(int A_B);
#endif

