/********************************************************************
	> File Name:	Format_Data.c
	> Author:		Austzhu
	> Mail:			153462902@qq.com.com
	> Created Time:	2016年04月22日 星期五 11时04分29秒
 *******************************************************************/
#include "Format_Data.h"
#include "Crc16.h"
#include "debug.h"
#include <string.h>
#include "Init.h"
#include "Update.h"
#include "Config.h"

Group_data   G_message;//分组信息

/**
 * 功能:  接收并解析一帧数据
 * Uartx 指定串口号
 * block 是否阻塞，<0一直阻塞，>0阻塞的时间，单位us
 * 返回值 成功返回 Success, 失败返回 Error, 地址不匹配返回 CoorAddr_Err
 */
s32 Analysis_Package(Uart_Port Uartx,s32 block)
{
	Pag_Single Single;
	memset(&Single,0,sizeof(Single));
	debug(DEBUG_Analysis,"*****Waiting Recv Package!*****\r\n");

	if(Success != RecvOnePackage(&Single,Uartx,block)){
		return Error;
	}

	switch(Single.Ctrl){
		case 0x01:Single_Ctrl(&Single);break;
		case 0x02:break;
		case 0x04:break;
		case 0x08:break;
		case 0x10:
		#ifdef Coordi_App
			Coordi_Set(&Single);break;
		#endif
		case 0x20:break;
		default:break;
	}
	return Success;
}

s32 RecvOnePackage(void *buf, Uart_Port Uartx,  s32 block)
{
	Pag_Single *Single = (Pag_Single*)buf;
	int TimeOut = 5;
	memset(Single,0,sizeof(Pag_Single));
	while(HeaderInfo != (0xff&Single->Header) && TimeOut--){
		Single->Header = RecvOneByte(Uartx,1000000);
	}
	if(TimeOut <= 0 && HeaderInfo != (0xff&Single->Header)){
		debug(DEBUG_Analysis,"<RecvOnePackage>Wait Header time out!\r\n");
		return -1;
	}
	if(Error == Recv_Package(&Single->Ctrl,sizeof(Pag_Single)-1,Uartx,block) ){
		debug(DEBUG_Analysis,"<RecvOnePackage>Recv_Package err!\r\n");
		return Error;
	}

	DisplayPackage("*************************",Single,sizeof(Pag_Single));
	if(Success != CHK_Crc16(Single->Crc16,(u8*)Single,sizeof(Pag_Single)-2) ){
		debug(DEBUG_Analysis,"<RecvOnePackage>Recv package CRC check err!\r\n");
		return Error;
	}
	return Success;
}

s32 Single_Ctrl(Pag_Single *Single)
{
#ifdef Coordi_App
	/* 匹配协调器地址 */
	if((Single->Coordi_Addr) != Init_Info.Addr){
		debug(DEBUG_Analysis,"Coordi_Addr Unmatch!\r\n");
		return Error;
	}
	/* 协调器向单灯发送数据 */
	Send_Package(Single, sizeof(Pag_Single),Uart2);
	/* 等待单灯回复数据 */
	if(Success != RecvOnePackage(Single,Uart2,2)){
		debug(DEBUG_Analysis,"Wait Uart2 Response ERROR!\r\n");
		return Error;
	}
	/* 打印单灯回复的数据，并且回复给集中控制器 */
	DisplayPackage("Recv data from Uart2",Single,sizeof(Pag_Single));
	Send_Package(Single,sizeof(Pag_Single),Uart1);
	/* 判断是否为单灯的升级命令 */
	if(Single->Cmd[1] == 0x80 && Single->Coordi_Addr == Init_Info.Addr){
		debug(DEBUG_Analysis,"Go to update single!\r\n");
		UpdateInfo.UpdateMySelf	= 0;
		UpdateInfo.ImageSize 		= 0;
		UpdateInfo.Repeat 		= 0;
		UpdateInfo.Is_UpdateImageA = (Single->Data[0] == 'A')? !0:0;
		Download_Application();
	}
#else
	u16 Cmd = 0;
	u32 itpos = 0, i = 0;
	/* 匹配单灯地址 */
	if(Init_Info.Addr != (Single->Single_Addr[0]<<8 | Single->Single_Addr[1])){
		debug(DEBUG_Analysis,"Single addr UnMatch!\r\n");
		return SingleAddr_Err;
	}
	Cmd = Single->Cmd[0] <<8 | Single->Cmd[1];
	//debug(DEBUG_Analysis,"Cmd is 0x%04x\r\n",Cmd);
	while(((itpos=0x01<<i++) != 0x10000) ){	//循环检测cmd每个bit位
		switch(Cmd & itpos){
			case 0x00:break;
			case 0x80://升级命令
				memset(&UpdateInfo,0,sizeof(UpdateInfo));
				if(!!Init_Info.Is_Coordi == !!Single->Data[0]){
					debug(DEBUG_Update,"Update Single!\r\n");
					UpdateInfo.UpdateMySelf = !0;
					if(!ImageA_Info.Is_Success){/* 回复上位机，要接收的升级镜像 */
						debug(DEBUG_Update,"Update Image B!\r\n");
						UpdateInfo.Is_UpdateImageA = 0;
						Single->Data[0] = 'B';
					}else{
						debug(DEBUG_Update,"Update Image A!\r\n");
						UpdateInfo.Is_UpdateImageA = !0;
						Single->Data[0] = 'A';
					}
					Single->Ctrl = 0x81;
					Crc16(Single->Crc16,(u8*)Single,sizeof(Pag_Single)-2);
					DisplayPackage("<Update>Single Respons Coordinate",Single,sizeof(Pag_Single));
					Send(Uart1,(char*)Single,sizeof(Pag_Single));
					Download_Application();
				}break;
			default: break;
		}
	}
#endif
	return Success;
}



s32 Recv_Package(void *buf,  u32 nLen, Uart_Port Uartx,  s32 block)
{
	int  ReadCnt  = 0;
	int  ReadSize = 0;
	u8 *ReadPtr = buf;
wait:
	ReadCnt = Recv(Uartx,(char*)(ReadPtr+ReadSize),nLen-ReadSize,1000000);
	ReadSize += ReadCnt;
	if(ReadSize < nLen && --block){ goto wait; }
	if(block <= 0){
		debug(DEBUG_Analysis,"Recv Package time out!\r\n");
		return Error;
	}
	return Success;
}

void Send_Package(const void *buf, u32 len, Uart_Port Uartx)
{
	Send(Uartx,buf,len);
}

void Group_dataInfo(void)
{
	I2C_EE_BufferRead((u8*)&G_message.Group_code,Group_addr,sizeof(G_message));
	if( CHK_Crc16(G_message.Crc,(u8*)&G_message.Group_code,sizeof(G_message) - 4) != Success){
		debug(DEBUG_Bootload,"Get G_message Crc16 check err!\r\n");
		I2C_EE_BufferRead((u8*)&G_message.Group_code,Group_bak,sizeof(G_message));
		if( CHK_Crc16(G_message.Crc,(u8*)&G_message.Group_code,sizeof(G_message) - 4) != Success){
				debug(DEBUG_Bootload,"Get G_message Crc16 check err form back!\r\n");
				G_message.Single_flag = 1;
		}
	}
 #ifdef Single_App
		if(G_message.Single_flag){
			Single_open;
			//debug(1,"^^^^^^^^open light!\r\n");
		}else{
			Single_close;
			//debug(1,"^^^^^^^^close light!\r\n");
		}
 #endif

}


#ifdef Coordi_App
s32 Coordi_Set(Pag_Single *Single)
{
	u16 Cmd = 0;
	u32 itpos = 0, i = 0;
	if(Init_Info.Addr != (Single->Coordi_Addr)){
		debug(DEBUG_Analysis,"Coordi addr UnMatch!\r\n");
		return CoorAddr_Err;
	}
	Cmd = Single->Cmd[0] <<8 | Single->Cmd[1];
	debug(DEBUG_Analysis,"Cmd is 0x%04x\r\n",Cmd);
	while(((itpos=0x01<<i++) != 0x10000) ){
		switch(Cmd & itpos){
			case 0x0000:break;
			case 0x0080:debug(DEBUG_Analysis,"Get Cmd Update!\r\n");
				memset(&UpdateInfo,0,sizeof(UpdateInfo));
				if(!!Init_Info.Is_Coordi == !!Single->Data[0]){
					debug(DEBUG_Update,"Update Coordinate!\r\n");
					UpdateInfo.UpdateMySelf = !0;
					/* 回复上位机，要接收的升级镜像 */
					if(!ImageA_Info.Is_Success){
						debug(DEBUG_Update,"Update Image B!\r\n");
						UpdateInfo.Is_UpdateImageA = 0;
						Single->Data[0] = 'B';
					}else{
						debug(DEBUG_Update,"Update Image A!\r\n");
						UpdateInfo.Is_UpdateImageA = !0;
						Single->Data[0] = 'A';
					}
					Single->Ctrl = 0x90;
					Crc16(Single->Crc16,(u8*)Single,sizeof(Pag_Single)-2);
					DisplayPackage("<Update>Coordinate Response CC",Single,sizeof(Pag_Single));
					Send(Uart1,(char*)Single,sizeof(Pag_Single));
					Download_Application();
				}break;
			default:debug(DEBUG_Analysis,"*****Cmd: 0x%04x Not Exist!\r\n",Cmd&itpos);
			break;
		}
	}
	return Success;
}
#endif

void DisplayPackage(const char *Message,void *Package,int nLen)
{
	u8 *PP = Package;
	debug(DEBUG_Printpkg,"%s:",Message);
	while(nLen--){
		debug(DEBUG_Printpkg,"0x%02x ",*PP++);
	}debug(DEBUG_Printpkg,"\r\n");
}

