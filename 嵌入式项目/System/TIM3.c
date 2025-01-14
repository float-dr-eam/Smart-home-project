#include "stm32f10x.h"  // Device header

void TIM3_Init(void)
{
    // ?? TIM1 ?? (TIM1 ? APB2 ?)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // ??????????
    TIM_InternalClockConfig(TIM3);
    
    // ???????????
    TIM_TimeBaseInitTypeDef TIM_TimerBaseInitStructure;
    TIM_TimerBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ????
    TIM_TimerBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // ??????
    TIM_TimerBaseInitStructure.TIM_Period = 10000 - 1; // ??????
    TIM_TimerBaseInitStructure.TIM_Prescaler = 3600 - 1; // ????
    TIM_TimerBaseInitStructure.TIM_RepetitionCounter = 0; // ?????
    TIM_TimeBaseInit(TIM3, &TIM_TimerBaseInitStructure);
    
    // ??????
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    
    // ??????
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // ?? NVIC ??????? (????)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // ??? NVIC ?????? TIM1 ????
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // ?? TIM1 ??????
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ?????
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // ????
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // ??????
    NVIC_Init(&NVIC_InitStructure);
    
    // ?????
    TIM_Cmd(TIM3, ENABLE);
}

//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
//	{
//		Num++;
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//	}
//}
	
