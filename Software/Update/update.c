#include "update.h"
#include <string.h>
#include "Interface.h"
#include "FlashProgram.h" 
#include "Crc16.h"
#include "Uart.h"

static u32 Updating;
Image_Info_t *P_ImageA_Info;
Image_Info_t *P_ImageB_Info;
Image_Info_t ImageA_Info;
Image_Info_t ImageB_Info;
Package_Info UpdateInfo;
Package_Data Update;

static s32 Recv_pkg(void)
{
	
	int ReadCnt			= 0;
	int ReadSize 		= 0;
	char *ReadPtr 		= (char*)(&Update.Coor_Addr);
	UpdateInfo.Time_out = 5;
	char temp;
	
go_on:
	ReadCnt 	 = Recv(Uart1,ReadPtr+ReadSize,sizeof(Update)-1-ReadSize,1000000);
	ReadSize	+= ReadCnt;
	if(ReadSize < sizeof(Update)-1 && --UpdateInfo.Time_out){
		goto go_on;
	}
	if(UpdateInfo.Time_out <= 0){
		debug(DEBUG_Update,"Recv Package time out!\r\n");
		return Fail;
	}
	
	/* 数据接收完毕 */
	if(UpdateInfo.UpdateMySelf){
		if(Init_Info.Is_Coordi){//协调器
			if((Init_Info.Addr&0xff) != Update.Coor_Addr){
				debug(DEBUG_Update,"Coordinate Addr Unmach\r\n");
				goto err;
			}
		}else{//单灯
			if( (Init_Info.Addr&0xffff) != (Update.Single_Addr_H<<8 | Update.Single_Addr_L) ){
				debug(DEBUG_Update,"Single Addr Unmach\r\n");
				goto err;
			}
		}
		
		if(Update.F_Num != (0xff&(~Update.__Num)) ){goto err;}				//校验数据帧
		if(Success == CHK_Crc16(Update.CRC16,Update.Data,sizeof(Update.Data))){//校验数据
			if(Success == FlashProgrm()){
				goto success;
			}else{ goto err; }
		}else{ 
			debug(DEBUG_Update,"#*#CRC16 check error!\r\n"); 
			goto err;
		}
	/* 协调器接收到单灯升级程序 单灯,把数据往下面转发 */
	}else{
		Send(Uart2,(char*)&Update.Header,sizeof(Update));
		int i=3;
		wait:
		switch((u8)RecvOneByte(Uart2,1000000)){
			case ACK:goto success;
			case NAK:goto err;
			case 0xF0:
				if(!(--i)){goto err;}
				goto wait;
		}
	}
	
	
success:
	temp = ACK; Send(Uart1,&temp,1);
	UpdateInfo.ImageSize += UPACKSIZE;
	debug(DEBUG_Update,"#");
	if(UpdateInfo.ImageSize > 50*1024){
		debug(DEBUG_Update,"Image over range!\r\n");
		/* 立马停止操作 */
		Updating = 0;
	}
	return Success;
err:
	debug(DEBUG_Update,"$");
	temp = NAK; Send(Uart1,&temp,1);
	return Error;	
}


void Download_Application(void)
{
	debug(DEBUG_Update,"<%s>%d.\r\n",__func__,__LINE__);
	u32 Uptiemout = 1;
	Updating = !0;
	while(Updating && Uptiemout){
		switch(Update.Header = RecvOneByte(Uart1,1000000)){
			case EOT:
				Updating = 0;
			case SOH:
				Uptiemout = 1;
				if(Success == Recv_pkg() && !Updating){
					debug(DEBUG_Update,"\n\rUpdate Image %c Success!\r\n",UpdateInfo.Is_UpdateImageA?'A':'B');
					debug(DEBUG_Update,"Get Frame Sum:%d\r\n",Update.F_Num+1);
					#ifdef Coordi_App
					if(0 == UpdateInfo.UpdateMySelf){
						//debug(1,"\n***^^&&&***&&&**update***^^^^&&&**\r\n");
						return;
					}else{
						Set_ImageInfo();
						Jump_Application();
					}
					#else
						Set_ImageInfo();
						Jump_Application();	
					#endif
				}else{
					Updating = ~0;
				}break;
			case CAN:return;
			case 0xf0:if(Uptiemout++ >5){Uptiemout = 0;}
			default:break;
		}
	}
}


