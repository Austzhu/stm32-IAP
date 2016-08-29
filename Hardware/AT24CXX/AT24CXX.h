/********************************************************************
    > File Name:    AT24CXX.h
    > Author:       Austzhu
    > Mail:         153462902@qq.com.com 
    > Created Time: 2016年06月07日 星期二 9时41分57秒
 *******************************************************************/

#ifndef __AT24CXX_H__
#define __AT24CXX_H__
#include "stm32f10x.h"
typedef struct{
	u32	Speed;
	u8	E2PROM_ADDR;
	u8	I2C_PAGESIZE;
	u8	I2C1_OWN_ADDRESS7;
} AT24CXX_INFO;



extern void AT24CXX_INIT(void);
extern void I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr);
extern void I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
extern void I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite);
extern void I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite);
extern void I2C_EE_WaitEepromStandbyState(void);
#endif //#ifndef __AT24CXX_H__
