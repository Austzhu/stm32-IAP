#include <stdarg.h>
#include <string.h>
#include "Uart.h"
#include "Init.h"

static UartResource RUART1;
static UartResource RUART2;


void ClearBuffer(void)
{
	int ii=0;
	RUART1.Wptr = 0;
	RUART1.Rptr = 0;
	RUART2.Rptr	= 0;
	RUART2.Wptr = 0;
	while(ii<RecvSize){
		
		RUART2.Buffer[ii] = 0;
		RUART1.Buffer[ii] = 0;
		++ii;
	}
}

void flush_Uart(int Uartx)
{
	int ii=0;
	switch(Uartx){
		case Uart1:
			ii = RecvSize;
			RUART1.Wptr = 0;
			RUART1.Rptr = 0;
			while(--ii){
				RUART1.Buffer[ii] = 0;
			}
			RUART1.Buffer[0] = 0;
			break;
		case Uart2:
			ii = RecvSize;
			RUART2.Wptr = 0;
			RUART2.Rptr = 0;
			while(--ii){
				RUART2.Buffer[ii] = 0;
			}
			RUART2.Buffer[0] = 0;
			break;
	}
}


/**
 * 配置串口波特率
 * cmd： 0x01为Uart1,0x02为Uart2，0X04为Uart3，可以使用位或操作同时设置串口
 * 	   后面更的参数必须为前面cmd指定串口按顺序填入
 * 返回值：无
 */
void USART_Config(u8 cmd,...)
{
	u32 baud[3] ={0};
	va_list arg_ptr;
	/* 获取波特率 */
	va_start(arg_ptr, cmd);
	if( cmd&UBaud_1 ){baud[0] = va_arg(arg_ptr, u32);} 
	if( cmd&UBaud_2 ){baud[1] = va_arg(arg_ptr, u32);}
	if( cmd&UBaud_3 ){baud[2] = va_arg(arg_ptr, u32);}
	va_end(arg_ptr); 

	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
 /*****************************UART1*****************************/	
 #ifdef CONFIG_Uart1

	/* 配置串口1 （USART1） 时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	/*串口GPIO端口配置*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* 485使能gpio */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	/* 配置串口1 USART1 Rx (PA.10)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;  	//抢占2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			// 优先1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
	
	/* 串口1工作模式（USART1 mode）配置 */
	USART_InitStructure.USART_BaudRate = baud[0];
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	
  
	USART_ClearFlag(USART1, USART_FLAG_RXNE);		  	//清零发送置位标准!!
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		//开启接受中断
	USART_Cmd(USART1, ENABLE);                    		//使能串口
	
 #endif
 
 /*****************************UART2*****************************/	
 #ifdef CONFIG_Uart2
	/* 配置串口2 （USART2） 时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	/*串口GPIO端口配置*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* 485使能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* 配置串口2 USART1 Rx (PA.3)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	//Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;	//	抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//	响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	/* 串口2工作模式（USART2 mode）配置 */
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = baud[1];
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	
	USART_ClearFlag(USART2, USART_FLAG_RXNE);		  	//清零发送置位 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//开启接受中断
	USART_Cmd(USART2, ENABLE);							//使能串口
 #endif
 
 /*****************************UART3*****************************/	
 #ifdef CONFIG_Uart3
	/* 配置串口1 （USART3） 时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
	/*串口GPIO端口配置*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	/* 配置串口3 USART3 Rx (PB.11)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;  	//抢占2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			// 优先1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
	
	
	/* 串口3工作模式（USART3 mode）配置 */
	USART_InitStructure.USART_BaudRate = baud[2];
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
	
	USART_ClearFlag(USART3, USART_FLAG_RXNE);		  	//清零发送置位标准!!
	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//开启接受中断
	USART_Cmd(USART3, ENABLE);							//使能串口
 #endif
 
	/* 清空串口缓存区数据 */
	USART_ReceiveData(USART1);
	USART_ReceiveData(USART2);
	USART_ReceiveData(USART3);
	ClearBuffer();
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
	USART_TypeDef *Uartx = (USART_TypeDef *)Print_Port;
	Uartx->SR &= ~(0x01<<6);				//清除SR中的TC位
	Uartx->DR = (ch & (uint16_t)0x01FF);
	while(!(Uartx->SR&(0x01<<6)));	
	return (ch);
}


/* 允许485为发送状态 */
void Enable_485(int Port)
{
	switch(Port){
		case Uart1: GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);break;
		case Uart2: GPIO_WriteBit(GPIOA, GPIO_Pin_4,  Bit_SET);break;
		case Uart3: break;
		default: break;
	}
}

/* 设置485为接收状态 */
void Disable_485(int Port)
{
	switch(Port){
		case Uart1: GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);break;
		case Uart2: GPIO_WriteBit(GPIOA, GPIO_Pin_4,  Bit_RESET);break;
		case Uart3: break;
		default: break;
	}
}


static u8 BufferRead(char *data,int Uartx)
{
	switch(Uartx){
		case Uart1:
			if(RUART1.Rptr == RUART1.Wptr){break;}
			*data = RUART1.Buffer[RUART1.Rptr++];
			RUART1.Rptr %= RecvSize;
			return 1;
		case Uart2:
			if(RUART2.Rptr == RUART2.Wptr){break;}
			*data = RUART2.Buffer[RUART2.Rptr++];
			RUART2.Rptr %= RecvSize;
			return 1;
		case Uart3:
			break;
		default:break;
	}
	
	return 0;
}

static void WriteBuffer(int Uartx)
{	
	switch(Uartx){
		case Uart1:
			if(RUART1.Wptr == RUART1.Rptr-1){break;}
			RUART1.Buffer[RUART1.Wptr] = USART_ReceiveData((USART_TypeDef *)Uartx);
			#ifdef Uart1_Print
				printf("%02x ",RUART1.Buffer[RUART1.Wptr]);
			#endif
			++RUART1.Wptr;
			RUART1.Wptr %= RecvSize;
			break;
		case Uart2:
			if(RUART2.Wptr == RUART2.Rptr-1){break;}
			RUART2.Buffer[RUART2.Wptr] = USART_ReceiveData((USART_TypeDef *)Uartx);
			#ifdef Uart2_Print
				printf("%02x ",RUART2.Buffer[RUART2.Wptr]);
			#endif
			RUART2.Wptr++;
			RUART2.Wptr %= RecvSize;
			break;
		case Uart3:
			break;
		default:break;
	}
}

int Recv(int Uartx, char *buf, int nLen, int block)
{
	char *Pbuf = buf;
	Disable_485(Uartx);  //485设置成接收模式
	while(nLen && block){
		if(BufferRead(Pbuf,Uartx)){
			++Pbuf;
			--nLen;
		}else{
			if(block > 0)
				--block;
			delay_us(1);
		}
	}
	return Pbuf - buf;
}


char RecvOneByte(int Uartx,int block)
{
	char ch;
	return Recv(Uartx,&ch,1,block)?ch:0xf0;
}

void Send(int Uartx,const char *buf, int nLen)
{
	Enable_485(Uartx);
	while(nLen--){
		USART_ClearFlag((USART_TypeDef *)Uartx,USART_FLAG_TC);
		USART_SendData((USART_TypeDef *)Uartx,*buf++);
		while( USART_GetFlagStatus((USART_TypeDef *)Uartx,USART_FLAG_TC)!= SET);
	}
}

void SendOneByte(int Uartx,const char ch)
{
	Send(Uartx,&ch,1);
}


/* Uart1中断服务函数 */
void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) != RESET)
		USART_ReceiveData(USART1);
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) != RESET){
		WriteBuffer(Uart1);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

/* Uart2中断服务函数 */
void USART2_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) != RESET)
		USART_ReceiveData(USART2);
	if(USART_GetFlagStatus(USART2,USART_IT_RXNE) != RESET){
		WriteBuffer(Uart2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

/* Uart3中断服务函数 */
void USART3_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) != RESET)
		USART_ReceiveData(USART3);
	if(USART_GetFlagStatus(USART3,USART_IT_RXNE) != RESET){
		WriteBuffer(Uart3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
}

