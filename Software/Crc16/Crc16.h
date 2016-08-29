#ifndef __CRC16_H__
#define __CRC16_H__
#include "stm32f10x.h"

extern s32 Crc16(unsigned char *crc,unsigned char *puchMsg, unsigned long usDataLen);
extern s32 CHK_Crc16(u8 *crc,  u8 *puchMsg,  s32 usDataLen);

#endif
