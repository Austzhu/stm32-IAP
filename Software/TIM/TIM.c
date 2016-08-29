#include "Tim.h"
#include "Init.h"

void Tim_Init(void)
{
	RCC->APB1ENR |= 0x01<<0;						// 开启TIM2时钟 
	//NVIC->IP[TIM2_IRQn] = 0xf0;						//0101 0000  高4位设置优先级，低4位没用。 抢占优先级01，子优先级01
    NVIC->ISER[TIM2_IRQn/32] |= 0x01<<TIM2_IRQn;	//使能TIM2中断
	
	/* 定时器2的初始化 */
	#ifdef CONFIG_TIM2
		vu16 TempCCMR1 	= 0, TempCCER	= 0;
	
		TempCCMR1 = 0X01<<4 | 0x1<<12;	//输出比较模式OC1M[2:0] 0x1,禁止TIMx_CCR1寄存器的预装载功能

		TempCCER  = 0x3<<0 | 0x03<<4 | 0x03<<8 |0x03<<12;	//每个通道设置为输出允许，低电平有效

		TIM2->CCMR1 = TempCCMR1;
		TIM2->CCMR2 = TempCCMR1;
		TIM2->CCER  = TempCCER;
	
		TIM2->CR2	= 0X00;				//MMS[2:0] 复位，
		TIM2->SMCR  = 0X00;				//使用内部时钟
		
		TIM2->PSC   = 7199;				//72MHZ/(7199+1) = 100000HZ ,100us
		TIM2->ARR	= BASE_VAL; 		//设置预装值
		
		/* 设置通道的比较初值 */
		TIM2->CCR1  = CCR1_Val;
		TIM2->CCR2  = CCR2_Val;
		TIM2->CCR3  = CCR3_Val;
		TIM2->CCR4  = CCR4_Val;

		TIM2->EGR   = 0X01;		//产生更新事件，使psc立即生效
		TIM2->SR 	= 0X00;		//清除所有状态标记位
		TIM2->CNT	= 0X00;		//初始化计数值
		
		TIM2->DIER  = 0X0F | 0X01<<4 | 0X01<<6;  //开启定时器的通道中断使能，禁用DMA中断
		/* 开启定时器 */
		TIM2->CR1	= 0X01;		//禁止预装缓冲ARPE，CMS[1:0] 边沿对齐，向上计数，在发生更新事件时，计数器不停止，允许所以更新请求源
	#endif
	
}

void TIM2_IRQHandler(void)
{
	static vu16 CNT = 0;
	static int Led_State =0;
	
	if(TIM2->SR&(0X01<<1)){
		TIM2->SR &= ~(0X01<<1);		//清中断标记位
		CNT = TIM2->CCR1;
		TIM2->CCR1 = (CNT+CCR1_Val)%BASE_VAL;
		//LED_OFF;
	}
	
	if(TIM2->SR&(0X01<<2)){
		TIM2->SR &= ~(0X01<<2);		//清中断标记位
		CNT = TIM2->CCR2;
		TIM2->CCR2 = (CNT+CCR2_Val)%BASE_VAL;
		//LED_ON;
	}
	
	if(TIM2->SR&(0X01<<3)){	
		TIM2->SR &= ~(0X01<<3);		//清中断标记位
		CNT = TIM2->CCR3;
		TIM2->CCR3 = (CNT+CCR3_Val)%BASE_VAL;
		//LED_OFF;
	}
	
	if(TIM2->SR&(0X01<<4)){
		TIM2->SR &= ~(0X01<<4);		//清中断标记位
		CNT = TIM2->CCR4;
		TIM2->CCR4 = (CNT+CCR4_Val)%BASE_VAL;
		if(Led_State){
			Led_State = !Led_State;
			LED_OFF;
		}else{
			Led_State = !Led_State;
			LED_ON;
		}
		
	}
}






