#ifndef __UART_H__
#define	__UART_H__
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "delay.h"
#include <stdio.h>
#include "Config.h"
#include "wdg.h"

#define CONFIG_Uart1

#ifdef Coordi_App
#define CONFIG_Uart2
#endif

#define CONFIG_Uart3
#define UBaud_1 0x01
#define UBaud_2 0x02
#define UBaud_3 0x04
#define RecvSize 	1024
#define BufSize2_3	512
#define Uart3_Baud 	57600	//串口3的波特率
#define Print_Port Uart3 	// 指定printf输出的串口 

typedef enum{
	Uart1 = USART1_BASE,
	Uart2 = USART2_BASE,
	Uart3 = USART3_BASE,
}Uart_Port;

typedef struct{
	char	Buffer[RecvSize];
	s32		Wptr;
	s32		Rptr;
}UartResource;
typedef struct{
	char	Buffer[BufSize2_3];
	s32		Wptr;
	s32		Rptr;
}USART2_3;



extern void	USART_Config(u8 cmd,...);
extern int	fputc(int ch, FILE *f);
extern void	Enable_485(int Port);
extern void	Disable_485(int Port);
extern int	Recv(int Uartx, char *buf, int nLen, int block);
extern char RecvOneByte(int Uartx,int block);
extern void	Send(int Uartx, const char *buf, int nLen);
extern void SendOneByte(int Uartx,const char ch);
extern void	ClearBuffer(void);
extern void flush_Uart(int Uartx);


#endif /* __UART_H__ */



