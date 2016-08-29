#ifndef __UPDATE_H__
#define __UPDATE_H__
#include "stm32f10x.h"
#include "Errno.h"
#include "Uart.h"
#include "debug.h"
#define Page_Size 			1024		//一页为1KB

#define ImageA_Info_Start	0X8005000
#define ImageA_Info_End		0X8005FFF
#define ImageA_Start 		0X8006000
#define ImageA_End	 		0X80127FF

#define ImageB_Info_Start	0X8012800
#define ImageB_Info_End		0X80137FF
#define ImageB_Start 		0X8013800
#define ImageB_End	 		0X801FFFF

#define SOH 				0x01
#define STX 				0x02
#define EOT 				0x04
#define ACK 				0x06
#define NAK 				0x15
#define CAN 				0x18
#define CTRLZ				0x1A

#define UPACKSIZE 			128

typedef struct{
	u32 	ImageSize;		//镜像大小
	s32 	Repeat;			//每帧错误重发次数
	s32 	Time_out;		//超时
	s32 	UpdateMySelf;		//是否升级自己
	s32 	Is_UpdateImageA;	//把升级文件写到A分区?
}Package_Info;

typedef struct{
	u8 	Header;
	u8 	Coor_Addr;
	u8 	Single_Addr_H;
	u8 	Single_Addr_L;
	u8 	F_Num;		//第几帧数据
	u8 	__Num;
	u8 	Data[UPACKSIZE];
	u8 	CRC16[2];	//低在前，高位在后
}Package_Data;

typedef struct{
	u32 Image_Size;
	u32 Is_Success;					//是否可以执行
	u32 Update_time;				//更新时间
}Image_Info_t;

extern Package_Info UpdateInfo;
extern Package_Data Update;
extern Image_Info_t *P_ImageA_Info;
extern Image_Info_t *P_ImageB_Info;
extern Image_Info_t ImageA_Info;
extern Image_Info_t ImageB_Info;

extern void Download_Application(void);



#endif 		//#ifndef __UPDATE_H__
