/********************************************************************
	> File Name:	Format_Data.h
	> Author:		Austzhu
	> Mail:			153462902@qq.com.com 
	> Created Time:	2016年04月22日 星期五 11时04分46秒
 *******************************************************************/
#ifndef __FORMAT_DATA_H__
#define __FORMAT_DATA_H__
#include "Include.h"
#include "Interface.h" 
#include "AT24CXX.h"

#define Group_addr		0x24
#define Group_bak		0x424
#define HeaderInfo 		0xff
#define Light_Irms 		1


typedef struct{
	u32 Group_code;
	u32 Map_addr;
	u32 Voltage;
	u32 Current;
	u32 Power;
	u32 Single_flag;
	u8  Crc[4];
}Group_data;

typedef struct{
	u8 Header;
	u8 Ctrl;
	u8 Group_Addr;
	u8 Coordi_Addr;
	u8 Single_Addr[2];
	u8 Cmd[2];
	u8 Data[2];
	u8 Crc16[2];
} Pag_Single;

extern Group_data   G_message;

extern void DisplayPackage(const char *Message,void *Package,int nLen);
extern s32 Recv_Package(void *buf,  u32 len, Uart_Port Uartx, s32 block);
extern void Send_Package(const void *buf, u32 len, Uart_Port Uartx);
extern s32 Analysis_Package(Uart_Port Uartx,s32 block);
extern s32 Single_Ctrl(Pag_Single *Single);
extern s32 Coordi_Set(Pag_Single *Single);
extern s32 RecvOnePackage(void *buf, Uart_Port Uartx,  s32 block);
extern void Group_dataInfo(void);

#endif 







