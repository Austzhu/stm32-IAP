/********************************************************************
    > File Name:    AT24CXX.c
    > Author:       Austzhu
    > Mail:         153462902@qq.com.com 
    > Created Time: 2016年06月07日 星期二 09时40分46秒
 *******************************************************************/

#include "AT24CXX.h"

AT24CXX_INFO at24c16={200000,0XA0,16,0X0A};

static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	/* 使能与 I2C1 有关的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
	/* PB6-I2C1_SCL、PB7-I2C1_SDA*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void I2C_Mode_Configu(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 

	/* I2C 配置 */
	I2C_InitStructure.I2C_Mode 					= I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle 			= I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 			= at24c16.I2C1_OWN_ADDRESS7; 
	I2C_InitStructure.I2C_Ack 					= I2C_Ack_Enable ;
	I2C_InitStructure.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed 			= at24c16.Speed;
  
	/* 使能 I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	/* I2C1 初始化 */
	I2C_Init(I2C1, &I2C_InitStructure);  
}

void AT24CXX_INIT(void)
{
	I2C_GPIO_Config();
	I2C_Mode_Configu();
	
}


void I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr)
{
	u8 Addr = at24c16.E2PROM_ADDR;
	Addr &= 0xF0;
	Addr |= 0x07&(WriteAddr>>8) << 1;
	
	I2C_GenerateSTART(I2C1, ENABLE);								//起始信号
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 	//等待应答 
	I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Transmitter);		//发送地址
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, WriteAddr&0xff);	//发送数据地址
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
	I2C_SendData(I2C1, *pBuffer&0xff); 		//写数据
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);		//停止
}


void I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
	u8 Addr = at24c16.E2PROM_ADDR;
	Addr &= 0xF0;
	Addr |= (0x07&(ReadAddr>>8)) << 1;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // 等待总线空闲
	I2C_GenerateSTART(I2C1, ENABLE);	//起始信号
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_Cmd(I2C1, ENABLE);
	I2C_SendData(I2C1, ReadAddr&0xff);  
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
	I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(NumByteToRead){
		if(NumByteToRead == 1){
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}

		if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){      
			*pBuffer = I2C_ReceiveData(I2C1);
			++pBuffer; 
			 --NumByteToRead; 
		}   
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);/* Enable Acknowledgement to be ready for another reception */
}


void I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
{
	u8 Addr = at24c16.E2PROM_ADDR;
	Addr &= 0xF0;
	Addr |= (0x07&(WriteAddr>>8)) << 1;
	
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); //等待总线空闲
   
	I2C_GenerateSTART(I2C1, ENABLE);/* Send START condition */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
  
	I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); 
  
	I2C_SendData(I2C1, WriteAddr&0xff);  
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* While there is data to be written */
	while(NumByteToWrite--){
		I2C_SendData(I2C1, *pBuffer); 
		++pBuffer; 
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}



void I2C_EE_WaitEepromStandbyState(void)      
{
	vu16 SR1_Tmp = 0;

	do{
		I2C_GenerateSTART(I2C1, ENABLE);
		SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
		I2C_Send7bitAddress(I2C1, at24c16.E2PROM_ADDR, I2C_Direction_Transmitter);
	}while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);    
    I2C_GenerateSTOP(I2C1, ENABLE); 
}




void I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
	u8 NumOfPage = 0, NumOfSingle = 0;
	
	NumOfPage 	= NumByteToWrite / at24c16.I2C_PAGESIZE;			//算出一共有多少页
	NumOfSingle = NumByteToWrite % at24c16.I2C_PAGESIZE;			//算出不够一页数据的余数
	
	if(!(WriteAddr%at24c16.I2C_PAGESIZE)){	//地址按页对齐
			while(NumOfPage--){
				I2C_EE_PageWrite(pBuffer,WriteAddr,at24c16.I2C_PAGESIZE);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr 	+= at24c16.I2C_PAGESIZE;
				pBuffer 	+= at24c16.I2C_PAGESIZE;
			}
			if(NumOfSingle){
				I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
				I2C_EE_WaitEepromStandbyState();
			}
		
	}else{//地址不按页对齐
		u32 nLen = at24c16.I2C_PAGESIZE - WriteAddr%at24c16.I2C_PAGESIZE;	//计算距离下个对齐页有多少个字节
		if(NumByteToWrite >nLen){
			NumByteToWrite 	-= nLen;
			NumOfPage 		 = NumByteToWrite / at24c16.I2C_PAGESIZE;
			NumOfSingle 	 = NumByteToWrite % at24c16.I2C_PAGESIZE;
		}else{
			NumOfPage 		 = 0;
			NumOfSingle		 = 0;
		}
		if(nLen){
			I2C_EE_PageWrite(pBuffer, WriteAddr, nLen);
			I2C_EE_WaitEepromStandbyState();
			WriteAddr 	+= nLen;
			pBuffer 	+= nLen;
		}
		while(NumOfPage--){
			I2C_EE_PageWrite(pBuffer,WriteAddr,at24c16.I2C_PAGESIZE);
			I2C_EE_WaitEepromStandbyState();
			WriteAddr 	+= at24c16.I2C_PAGESIZE;
			pBuffer 	+= at24c16.I2C_PAGESIZE;
		}
		if(NumOfSingle){
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_EE_WaitEepromStandbyState();
		}
	}
}









