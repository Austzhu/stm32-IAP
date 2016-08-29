#include "pwm.h"
#include "delay.h"
#include "Config.h"


Light_t Light[]=
{
	{0x00,1},{0x01,4},{0x02,7},{0x03,10},{0x04,13},{0x05,16},{0x06,19},{0x07,22},
	{0x08,25},{0x09,28},{0x0A,31},{0x0B,34},{0x0C,37},{0x0D,40},{0x0E,43},{0x0F,46},
	{0x10,49},{0x11,52},{0x12,55},{0x13,58},{0x16,61},{0x17,64},{0x18,67},{0x19,70},
	{0x1A,73},{0x1B,76},{0x1C,79},{0x1D,82},{0xFF,0XFF}
};

void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  
	
#ifdef Single_App
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//K1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure); 
#endif
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM4_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //CH1 PWM2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //OC3
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3
	//TIM_ARRPreloadConfig(TIM4, ENABLE); 
	TIM_Cmd(TIM4, ENABLE);  //??TIMx  										  
} 





