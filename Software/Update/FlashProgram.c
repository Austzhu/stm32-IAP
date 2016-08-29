#include "FlashProgram.h"
#include "update.h"
#include <string.h>

#if 0
void FLASHImageInfoErase(char ch)
{
	int n= 0;
	while(n<(ImageA_Start -ImageA_Info_Start)/Page_Size){
		if(ch){//擦除A
			FLASH_ErasePage(ImageA_Info_Start+Page_Size*n++);
		}else{
			FLASH_ErasePage(ImageB_Info_Start+Page_Size*n++);
		}			
	}
	
}

void FlashAllErase(u8 Image)
{
	int n = 0;
	
	while(n< (ImageA_End-ImageA_Info_End)/Page_Size){
		if(Image){
			FLASH_ErasePage(ImageA_Start+Page_Size*n++);
		}else{
			FLASH_ErasePage(ImageB_Start+Page_Size*n++);
		}
	}
}

void CopyFromFlash(void *dest,vu32 SrcAddr)
{
	if(FLASH_GetReadOutProtectionStatus() != RESET){  
		FLASH_Unlock();  
		FLASH_ReadOutProtection(DISABLE);  
	}
	char *Pdest = dest;
	char *Ptr  = (char*)SrcAddr;
	while((Pdest-(char*)dest) < Page_Size){
		printf("*%02x ",*Pdest);
		*Pdest++ = *Ptr++;
	}
	printf("\r\n\n\n");
	
}
void CopyToBuf(void *Dest,const void *Src,int nLen)
{
	char *dest = Dest;
	char *pSRC = (char*)Src;
	while(dest-(char*)Dest < nLen){
		printf("*%02x ",*dest);
		*dest++ = *pSRC++;
	}
	
}
#endif



int FlashProgrm(void)
{
	
	int ErrorCnt = 0;
	FLASH_Status status = FLASH_COMPLETE;
	
#if 0
	vu32 Page_Start = Update.F_Num - Update.F_Num%(Page_Size/UPACKSIZE);
	u8 OnePage[1024];
	u16 *Ptr = (u16*)OnePage;
	vu32 WriteAddr = UpdateInfo.Is_UpdateImageA ? Page_Start*UPACKSIZE+ImageA_Start:Page_Start*UPACKSIZE+ImageB_Start;
	vu32 WriteBuf  = Update.F_Num%(Page_Size/UPACKSIZE)*UPACKSIZE;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	
	//读取flash中一页的数据
	memcpy(OnePage,(void*)WriteAddr,Page_Size);
	memcpy(OnePage+WriteBuf,Update.Data,UPACKSIZE);
	//擦除一页的flash
	status = FLASH_ErasePage(WriteAddr);
	//debug(DEBUG_Program,"Erase Page Addr:0x%08X  status=%d\r\n",WriteAddr,status);
	
	while((u32)Ptr < (u32)(OnePage+1024)){
		status = FLASH_ProgramHalfWord(WriteAddr,*Ptr);
		if(FLASH_COMPLETE ==status){
			++Ptr; WriteAddr += 2;
			ErrorCnt =0;
		}else{
			++ErrorCnt;
			debug(1,"status=%d\r\n",status);
		}
		if(ErrorCnt > 10){//连续5次都不能把一个Word写入flash则返回错误！
			FLASH_Lock();
			debug(DEBUG_Program,"FLASH_ProgramWord error!\r\n");
			return Error;
		}
	}
#else
	u16 *Ptr = (u16*)Update.Data;
	vu32 WriteAddr = UpdateInfo.Is_UpdateImageA ? Update.F_Num*UPACKSIZE+ImageA_Start:\
												Update.F_Num*UPACKSIZE+ImageB_Start;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	if(!(Update.F_Num%(Page_Size/UPACKSIZE))){
		status = FLASH_ErasePage(WriteAddr);
		if( FLASH_COMPLETE !=status ){
			debug(DEBUG_Program,"Erase Page Addr:0x%08X  status=%d\r\n",WriteAddr,status);
		}
		
	}
	while((u32)Ptr < (u32)Update.CRC16){
		status = FLASH_ProgramHalfWord(WriteAddr,*Ptr);
		if(FLASH_COMPLETE ==status){
			++Ptr;
			WriteAddr += 2;
			ErrorCnt =0;
		}else{
			++ErrorCnt;
		}
		
		if(ErrorCnt > 10){//连续5次都不能把一个Word写入flash则返回错误！
			FLASH_Lock();
			debug(DEBUG_Program,"FLASH_ProgramWord error!\r\n");
			return Error;
		}
	}
#endif
	
	FLASH_Lock();
	return Success;
}

void Get_ImageInfo(void)
{
	if(FLASH_GetReadOutProtectionStatus() != RESET){  
		FLASH_Unlock();  
		FLASH_ReadOutProtection(DISABLE);  
	}
	P_ImageA_Info = (Image_Info_t*)ImageA_Info_Start;
	P_ImageB_Info = (Image_Info_t*)ImageB_Info_Start;
	
	ImageA_Info.Image_Size	= P_ImageA_Info->Image_Size;
	ImageA_Info.Is_Success	= P_ImageA_Info->Is_Success;
	ImageA_Info.Update_time = P_ImageA_Info->Update_time;
	
	ImageB_Info.Image_Size	= P_ImageB_Info->Image_Size;
	ImageB_Info.Is_Success	= P_ImageB_Info->Is_Success;
	ImageB_Info.Update_time = P_ImageB_Info->Update_time;
	debug(DEBUG_ImageInfo,"ImageA_Info:\r\n\tImage_Size=%d\r\n\tIs_Success=%x\r\n\tUpdate_time=%x\r\n",
		ImageA_Info.Image_Size,ImageA_Info.Is_Success,ImageA_Info.Update_time);
	debug(DEBUG_ImageInfo,"ImageB_Info:\r\n\tImage_Size=%d\r\n\tIs_Success=%x\r\n\tUpdate_time=%x\r\n",
		ImageB_Info.Image_Size,ImageB_Info.Is_Success,ImageB_Info.Update_time);
}

#if 0
void Set_ImageInfo_A(int A_B)
{
	int i=0;	
	u32 Wptr = ImageA_Info_Start;
	u16 *Rptr = (u16*)(&ImageA_Info.Image_Size);
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	/* 写Image_A的信息 */
	if(A_B){
		if(FLASH_COMPLETE != FLASH_ErasePage(ImageA_Info_Start)){
			debug(DEBUG_ImageInfo,"Erase Image A Info Start err!\r\n");
		}
		while(i < sizeof(Image_Info_t)){
			if(FLASH_COMPLETE == FLASH_ProgramHalfWord(Wptr,*Rptr)){
				Wptr += 2;i += 2;++Rptr;
			}else{
				debug(DEBUG_ImageInfo,"FLASH Program A ERR!\r\n");
				break;
			}
		}
	/* 写Image_B的信息 */
	}else{
		Wptr = ImageB_Info_Start;
		Rptr = (u16*)(&ImageB_Info.Image_Size);
		if(FLASH_COMPLETE != FLASH_ErasePage(ImageB_Info_Start)){
			debug(DEBUG_ImageInfo,"Erase Image B Info Start err!\r\n");
		}
		while(i < sizeof(Image_Info_t)){
			if(FLASH_COMPLETE == FLASH_ProgramHalfWord(Wptr,*Rptr)){
				Wptr += 2;i += 2;++Rptr;
			}else{
				debug(DEBUG_ImageInfo,"FLASH Program B ERR!\r\n");
				break;
			}
		}
	}
	FLASH_Lock();	
}
#endif

void Set_ImageInfo(void)
{
	int i=0;	
	u32 Wptr = ImageA_Info_Start;
	u16 *Rptr = (u16*)(&ImageA_Info);
	/* 把升级后是文件大小，及更新时间写入Flash中去 */
	if(UpdateInfo.Is_UpdateImageA){	//Set Image A Info
		ImageA_Info.Update_time = ImageB_Info.Update_time +1;
		ImageA_Info.Image_Size 	= UpdateInfo.ImageSize;
	}else{
		ImageB_Info.Update_time = ImageA_Info.Update_time +1;
		ImageB_Info.Image_Size 	= UpdateInfo.ImageSize;
	}
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	/* 写A区的信息 */
	if(FLASH_COMPLETE != FLASH_ErasePage(ImageA_Info_Start)){
		debug(DEBUG_ImageInfo,"Erase Image B Info Start err!\r\n");
	}
	i=0; 
	while(i < sizeof(Image_Info_t)){
		if(FLASH_COMPLETE == FLASH_ProgramHalfWord(Wptr,*Rptr)){
			Wptr += 2;i += 2;++Rptr;
		}else{
			debug(DEBUG_ImageInfo,"FLASH Program A ERR!\r\n");
			break;
		}
	}
	/* 写B区的信息 */
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
	FLASH_Lock();
	/* 临时把改变Image的信息，没有写到flash中 */
	if(UpdateInfo.Is_UpdateImageA){	//Set Image A Info
		ImageA_Info.Is_Success 	= 0;
		ImageB_Info.Is_Success 	= ~0;
	}else{	//Set Image B Info
		ImageA_Info.Is_Success 	= ~0;
		ImageB_Info.Is_Success 	= 0;
	}
}


#if 0
void RCC_Config(void)
{
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	if(SUCCESS == HSEStartUpStatus){
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08);
	}
	RCC_HSICmd(ENABLE);
	
}
#endif


