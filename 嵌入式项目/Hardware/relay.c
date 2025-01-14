#include "Delay.h"
#include "Water.h"

#define RELAY_IO GPIO_Pin_4
#define RELAYPRO_IO GPIO_Pin_5
#define RELAY_PORT GPIOB
#define RELAY_RCC RCC_APB2Periph_GPIOB

void Relay_Init(void)
{ 	
	RCC_APB2PeriphClockCmd(RELAY_RCC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure; 
	
    GPIO_InitStructure.GPIO_Pin = RELAY_IO | RELAYPRO_IO;                       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);          
}				  

void relay_on1(void)
{
	GPIO_ResetBits(RELAY_PORT, RELAY_IO);
	
}

void relay_off1(void)
{
	GPIO_SetBits(RELAY_PORT, RELAY_IO);
}

void relay_on2(void)
{
	GPIO_ResetBits(RELAY_PORT, RELAYPRO_IO);
}

void relay_off2(void)
{
	GPIO_SetBits(RELAY_PORT, RELAYPRO_IO);
}




