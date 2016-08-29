#include "sys.h"		     
#include "delay.h"
#include "Address.h"
#include "stdio.h"

void ADDR_Init(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	/* 关闭JTAGD,启用GPIOB_4引脚 */
	AFIO->MAPR &= ~(0x07<<24);
	AFIO->MAPR |= 0x01<<24;
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void Get_Address(vu8 *Addr, u8 *Baud)
{	
	u8 i = 0, Result = 0 ,Result1 = 0;	
	/* 获取单灯的地址 */
	PL1_H;PL2_H;
	CLK1_L;CLK2_L;
	delay_us(5);
	PL1_L;PL2_L;
	delay_us(5);
	PL1_H;PL2_H;
	delay_us(5);
	
	for(i=0;i<8;i++){
		Result  <<=1;Result1 <<=1;
		CLK1_L;CLK2_L;
		delay_us(5);
		Result  |= Data1&0x01;
		Result1 |= Data2&0x01; 
    delay_us(5);		
		CLK1_H;CLK2_H;
		delay_us(5);
	}

	*Addr = 0xff&~Result;
	*Baud = 0x0f&~Result1;

}








