#include "Interface.h"
#include "stm32f10x.h"
#include "uart.h"
#include "debug.h"
#include "Init.h"
#include "AT24CXX.h"
#include "Crc16.h"
#include "FlashProgram.h"

Global_Info_t Init_Info;


void Init_BaseInfo(void)
{
	I2C_EE_BufferRead((u8*)&Init_Info.Addr,Base_Info_Addr,sizeof(Global_Info_t));
	if( CHK_Crc16(Init_Info.Crc,(u8*)&Init_Info.Addr,sizeof(Init_Info) - 4) != Success){
			debug(DEBUG_Bootload,"Get Init_Info Crc16 check err!\r\n");
		I2C_EE_BufferRead((u8*)&Init_Info.Addr,Base_Info_back,sizeof(Global_Info_t));
		if( CHK_Crc16(Init_Info.Crc,(u8*)&Init_Info.Addr,sizeof(Init_Info) - 4) != Success){
				debug(DEBUG_Bootload,"Get Init_Info Crc16 check err form back,user defalut!\r\n");
			Init_Info.Addr 		= 0x01;
			Init_Info.Is_Coordi = ~0;
			Init_Info.Bound[0] 	= Init_Info.Bound[1] = 9600;
			Init_Info.Bound[2]  = 57600;
			Init_Info.Voltage_K = 1.00;
		}
	}	
}


void Jump_Application(void)
{
	volatile u32 Jump_Addr;
	volatile u32 Jump;
	
	if(0 == ImageA_Info.Is_Success){
		Jump_Addr = ImageA_Start+4;
		if(0 == ImageB_Info.Is_Success && ImageB_Info.Update_time > ImageA_Info.Update_time){
			Jump_Addr = ImageB_Start+4;
		}
	}else if(0 == ImageB_Info.Is_Success){
		Jump_Addr = ImageB_Start+4;
	}else{
		Jump_Addr = ImageA_Start+4;
	}
	debug(DEBUG_Bootload,"Jump to 0x%x\r\n",Jump_Addr);
	Jump = *(vu32*)Jump_Addr;
	debug(DEBUG_Bootload,"*(vu32*)Jump_Addr = 0x%x\r\n",*(vu32*)(Jump_Addr-4));
	if((0x2FFE0000&(*(vu32*)(Jump_Addr-4))) == 0X20000000){
		((void(*)(void))Jump)();		
	}else{
		debug(DEBUG_Bootload,"Error:*(vu32*)%X = 0x%x\r\n",Jump_Addr-4,*(vu32*)(Jump_Addr-4));
		SoftReset();
	}
}

