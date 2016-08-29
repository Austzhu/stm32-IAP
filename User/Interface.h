#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include "update.h"

#define Base_Info_Addr 		0x00
#define Base_Info_back 		0x400

typedef struct{
	u32 Addr;
	u32 Bound[3];
	u32 Is_Coordi;			//标记是单灯还是协调器
	float Voltage_K;
	u8  Crc[4];
}Global_Info_t;

extern Global_Info_t Init_Info;

extern void Jump_Application(void);
extern void Init_BaseInfo(void);

#endif //#ifndef __INTERFACE_H__


