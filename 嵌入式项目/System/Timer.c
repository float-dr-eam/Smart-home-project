#include "stm32f10x.h"  // Device header

void Timer_Init(void)
{
    // ?? TIM1 ?? (TIM1 ? APB2 ?)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    // ??????????
    TIM_InternalClockConfig(TIM1);
    
    // ???????????
    TIM_TimeBaseInitTypeDef TIM_TimerBaseInitStructure;
    TIM_TimerBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ????
    TIM_TimerBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // ??????
    TIM_TimerBaseInitStructure.TIM_Period = 10000 - 1; // ??????
    TIM_TimerBaseInitStructure.TIM_Prescaler = 7200 - 1; // ????
    TIM_TimerBaseInitStructure.TIM_RepetitionCounter = 0; // ?????
    TIM_TimeBaseInit(TIM1, &TIM_TimerBaseInitStructure);
    
    // ??????
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    
    // ??????
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    
    // ?? NVIC ??????? (????)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // ??? NVIC ?????? TIM1 ????
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; // ?? TIM1 ??????
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ?????
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // ????
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // ??????
    NVIC_Init(&NVIC_InitStructure);
    
    // ?????
    TIM_Cmd(TIM1, ENABLE);
}

//void TIM2_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
//	{
//		Num++;
//		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
//	}
//}
	
