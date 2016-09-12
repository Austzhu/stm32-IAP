#include "Init.h"
#include <string.h>
#include "core_cm3.h"
#include "TIM.h"
#include "AT24CXX.h"
#include "Interface.h"
#include "FlashProgram.h"

void SoftReset(void)
{  
	__set_FAULTMASK(1); 
	NVIC_SystemReset();
}

static void K_Init(void)
{
#ifdef Single_App
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//K1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//GPIO_SetBits(GPIOA,GPIO_Pin_3); 
#endif
}
void Haedware_Init(void)
{	
	USART_Config(UBaud_1|UBaud_2|UBaud_3,9600,9600,57600);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();																				
	AT24CXX_INIT();																		
	Init_BaseInfo();	//获取波特率等基础信息
	LED_Init();	
	K_Init();
	Group_dataInfo();
	USART_Config(UBaud_1|UBaud_2|UBaud_3,Init_Info.Bound[0],Init_Info.Bound[1],Init_Info.Bound[2]);
	debug(DEBUG_Init,"Addr=0x%04x,Bound:Uart1=%d,Uart2=%d,Uart3=%d,Voltage_K:%f,Is_Coordi: %s\r\n",Init_Info.Addr,
			Init_Info.Bound[0],Init_Info.Bound[1],Init_Info.Bound[2],Init_Info.Voltage_K,Init_Info.Is_Coordi?"yes":"no");
	
	memset(&UpdateInfo,0,sizeof(UpdateInfo));
	Get_ImageInfo();
	debug(DEBUG_Init,"&Haedware_Init=%p\r\n",&Haedware_Init);
}


#if 0
void setImageIn(void)
{
	int i=0;	
	u32 Wptr = ImageA_Info_Start;
	u16 *Rptr = (u16*)(&ImageA_Info);
	
	if( ((u32)&setImageIn) >= 0X8013800){
		ImageB_Info.Is_Success = 0;
		ImageA_Info.Is_Success = ~0;
		debug(1,"BBBBBBBBBBBB\r\n");
	}else if( (u32)&setImageIn >= 0X8006000 && (u32)&setImageIn <= 0X8010000){
		ImageB_Info.Is_Success = ~0;
		ImageA_Info.Is_Success = 0;
		debug(1,"AAAAAAAAAAAA\r\n");
	}else{
		debug(1,"kkkkkkkkkkkkkk\r\n");
		ImageA_Info.Image_Size = 0;
		ImageB_Info.Image_Size = 0;
	}
	
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	
	if(FLASH_COMPLETE != FLASH_ErasePage(ImageA_Info_Start)){
		debug(DEBUG_ImageInfo,"Erase Image B Info Start err!\r\n");
	}
	
	//FLASH_ErasePage(ImageA_Info_Start);
	//debug(1,"FLASH_ErasePage(ImageA_Info_Start) is Success? %s\r\n",(FLASH_COMPLETE == FLASH_ErasePage(ImageA_Info_Start)) ? "yes":"no");
	i=0;
	while(i < sizeof(Image_Info_t)){
		if(FLASH_COMPLETE == FLASH_ProgramHalfWord(Wptr,*Rptr)){
			Wptr += 2;i += 2;++Rptr;
		}else{
			debug(DEBUG_ImageInfo,"FLASH Program A ERR!\r\n");
			break;
		}
	}
	
	
	Wptr = ImageB_Info_Start;
	Rptr = (u16*)(&ImageB_Info);
	if(FLASH_COMPLETE != FLASH_ErasePage(ImageB_Info_Start)){
		debug(DEBUG_ImageInfo,"Erase Image B Info Start err!\r\n");
	}
	i=0;
	while(i < sizeof(Image_Info_t)){
		if(FLASH_COMPLETE == FLASH_ProgramHalfWord(Wptr,*Rptr)){
			Wptr += 2;i += 2;++Rptr;
		}else{
			debug(DEBUG_ImageInfo,"FLASH Program B ERR!\r\n");
			break;
		}
	}
	
	
	Get_ImageInfo();
	FLASH_Lock();
	
}
#endif




