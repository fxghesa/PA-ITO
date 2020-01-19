#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f4xx_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f4xx_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "RTE_Device.h"                 // Keil::Device:Startup
#include "stdio.h"
static __IO uint32_t sysTickCounter;
char temp,  data_received[6], data_delay[6];
int y=0,i=0, x=0, ndelay=0,ii=0,c=0;
int delay1=0,delay2=0,delay3=0,delay4=0;
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stdio.h"

void USART_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);	
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

void USART_putchar(uint8_t ch)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void USART_puts(char* ch)
{
	uint8_t i;
	for(i=0;ch[i]!=0;i++)
	{
		USART_putchar(ch[i]);
	}
}

uint8_t USART_getchar(void)
{
	uint8_t ch;
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	ch = USART_ReceiveData(USART1);
	return ch;
}

void TIM4_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_PCLK1Config(RCC_HCLK_Div16);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12, GPIO_AF_TIM4); //pwm ch1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13, GPIO_AF_TIM4); //pwm ch2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14, GPIO_AF_TIM4); //pwm ch3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15, GPIO_AF_TIM4); //pwm ch4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	//fpwm=(10.5MHz*2)/(210*100)=1000Hz
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 210-1;
	TIM_TimeBaseInitStruct.TIM_Period = 100-1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM4,ENABLE);
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1; //mode non inverting
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM4,&TIM_OCInitStruct);
	TIM_OC2Init(TIM4,&TIM_OCInitStruct);
	TIM_OC3Init(TIM4,&TIM_OCInitStruct);
	TIM_OC4Init(TIM4,&TIM_OCInitStruct);
	TIM_Cmd(TIM4,ENABLE);
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM4,TIM_Channel_1,TIM_CCx_Enable);
	TIM_CCxCmd(TIM4,TIM_Channel_2,TIM_CCx_Enable);
	TIM_CCxCmd(TIM4,TIM_Channel_3,TIM_CCx_Enable);
	TIM_CCxCmd(TIM4,TIM_Channel_4,TIM_CCx_Enable);
}

void NVIC_Config(){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void USART1_IRQHandler(void){
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET){
		ii=0;
		y=1;
		while(1){
			temp = USART_getchar();
			if(temp=='!')break;
			data_received[ii] = temp;
			ii++;
		}
		for(c=0;c<6-1;c++){
			if(c<ii-1)data_delay[c]=data_received[c+1];
			else data_delay[c]=0;
		}
		sscanf(data_delay, "%d", &ndelay);
	}
}
int main(void){	
	unsigned char counts=0;
	USART_Config();
	TIM4_Config();
	NVIC_Config();
	for(;;){		
		if(data_received[0] == '1')TIM_SetCompare1(TIM4,ndelay);
		else if(data_received[0] == '2')TIM_SetCompare2(TIM4,ndelay);
		else if(data_received[0] == '3')TIM_SetCompare3(TIM4,ndelay);
		else if(data_received[0] == '4')TIM_SetCompare4(TIM4,ndelay);
	}
}
