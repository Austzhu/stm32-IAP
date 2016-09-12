#include "Include.h"
#include "Init.h" 
#include "AT24CXX.h"
#include "Interface.h"
#include "Uart.h"
#include "Crc16.h"
#include "FlashProgram.h"
#include "update.h"


int main(void)
{
	int Wait = 3;
	Haedware_Init();
	
#if 0
	Init_Info.Addr = 0x70;
	Init_Info.Bound[0]=Init_Info.Bound[1]=9600;
	Init_Info.Bound[2]=57600;
	Init_Info.Is_Coordi = 1;
	Init_Info.Voltage_K=1.00;
	Crc16(Init_Info.Crc,(u8*)&Init_Info,sizeof(Init_Info)-4);
	I2C_EE_BufferWrite((u8*)&Init_Info, Base_Info_Addr,sizeof(Init_Info));
	delay_ms(100);
	I2C_EE_BufferWrite((u8*)&Init_Info, Base_Info_back,sizeof(Init_Info));
#endif	
	while(Wait--){	
		Analysis_Package(Uart1,2);
	}
	Jump_Application();
}

