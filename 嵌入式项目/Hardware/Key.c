#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	//开启EXIT和AFIO
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12|GPIO_PinSource13|GPIO_PinSource14|GPIO_PinSource15);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //开启端口复用，涉及到GPIO口做外部中断时都需要这一条语句
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//将PB1和PB11引脚初始化为上拉输入

	NVIC_InitTypeDef NVIC_I;                           //定义初始化结构体
	EXTI_InitTypeDef EXTI_I;                           //定义初始化结构体
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置整个系统的中断优先级分组
	NVIC_I.NVIC_IRQChannel=EXTI15_10_IRQn;                 //设置初始化哪个中断
	NVIC_I.NVIC_IRQChannelPreemptionPriority=2;     //设置中断抢占优先级
	NVIC_I.NVIC_IRQChannelSubPriority=3;            //设置中断响应优先级（子优先级）
	NVIC_I.NVIC_IRQChannelCmd=ENABLE;                  //中断使能（启动）
	NVIC_Init(&NVIC_I);                                //初始化

	EXTI_I.EXTI_Line=EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15;                       //设置初始化哪条中断/事件线
	EXTI_I.EXTI_Mode = EXTI_Mode_Interrupt;            //设置为产生中断（EXTI_Mode_Event为产生事件）
	EXTI_I.EXTI_Trigger = EXTI_Trigger_Falling;        //设置为下降沿触发
	EXTI_I.EXTI_LineCmd = ENABLE;                      //使能
	EXTI_Init(&EXTI_I);                                //初始化

}
/*
void EXTI15_10_IRQHandler(void)
{
    uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)			//读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 1;												//置键码为1
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 2;												//置键码为2
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 3;												//置键码为2
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 4;												//置键码为2
	}
	EXTI_ClearITPendingBit(EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15);
}*/




/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
//uint8_t Key_GetNum(void)
//{
//	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
//	
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)			//读PB1输入寄存器的状态，如果为0，则代表按键1按下
//	{
//		Delay_ms(20);											//延时消抖
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);	//等待按键松手
//		Delay_ms(20);											//延时消抖
//		KeyNum = 1;												//置键码为1
//	}
//	
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
//	{
//		Delay_ms(20);											//延时消抖
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);	//等待按键松手
//		Delay_ms(20);											//延时消抖
//		KeyNum = 2;												//置键码为2
//	}
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
//	{
//		Delay_ms(20);											//延时消抖
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);	//等待按键松手
//		Delay_ms(20);											//延时消抖
//		KeyNum = 3;												//置键码为2
//	}
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
//	{
//		Delay_ms(20);											//延时消抖
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);	//等待按键松手
//		Delay_ms(20);											//延时消抖
//		KeyNum = 4;												//置键码为2
//	}
//	
//	return KeyNum;			//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
//}
