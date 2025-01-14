#include "Delay.h"  // 延时函数头文件
#include "Water.h"  // 水位相关的函数头文件
uint8_t min_water_leavel = 30;  // 最低水位阈值（可能用于水位检测）
void Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;  // 定义一个ADC初始化结构体，用于配置ADC的参数
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义一个GPIO初始化结构体，用于配置引脚
	// 启用ADC1和相关GPIO的时钟
	RCC_APB2PeriphClockCmd(WATER_RCC | RCC_APB2Periph_ADC1, ENABLE);  // 启用与ADC1和水位检测相关的外设时钟
	// 配置ADC时钟源为PCLK2的1/6，PCLK2是72MHz，因此ADC时钟为12MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // 设置ADC时钟为12MHz
	// 配置GPIO引脚（例如PA2）为模拟输入模式，供ADC采样使用
	GPIO_InitStructure.GPIO_Pin = WATER_IO;  // 配置水位检测引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 设置为模拟输入模式
	GPIO_Init(WATER_PORT, &GPIO_InitStructure);  // 初始化GPIO
	// 重置ADC1，清除其内部设置，以确保新的配置不会受到之前设置的影响
	ADC_DeInit(ADC1);  // 重置ADC1寄存器
	// 配置ADC的参数
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // 设置ADC工作模式为独立模式（只有ADC1工作）
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  // 禁用扫描转换模式（单通道转换）
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 禁用连续转换模式（手动触发转换）
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 不使用外部触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 设置数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;  // 只配置1个通道（假设是水位检测通道）
	ADC_Init(ADC1, &ADC_InitStructure);  // 初始化ADC1并应用配置
	// 启用ADC1
	ADC_Cmd(ADC1, ENABLE);  // 启动ADC1工作
	// 校准ADC1
	ADC_ResetCalibration(ADC1);  // 重置校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1));  // 等待校准寄存器重置完成
	ADC_StartCalibration(ADC1);  // 开始校准ADC1
	while (ADC_GetCalibrationStatus(ADC1));  // 等待校准完成
}				  

// ch: 通道号
u16 Get_Adc(u8 ch)   
{
  	// 配置ADC的通道和采样时间（这里使用了239.5个时钟周期的采样时间）
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);  // 配置ADC1进行通道ch的采样
	
	// 启动ADC转换
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 启动ADC的转换

	// 等待转换完成，直到ADC数据寄存器就绪
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  // 等待ADC转换结束（EOC标志位）

	// 返回ADC转换的结果
	return ADC_GetConversionValue(ADC1);  // 获取ADC1转换的值
}
 
// 获取ADC通道ch的平均值，通过重复多次读取并取平均值来减少噪声
// ch: 通道号
// times: 读取次数，通常可以设为8次、16次等
u16 Get_Adc_Average(u8 ch, u8 times)
{
	u32 temp_val = 0;  // 存储累加的ADC值
	u8 t;
	// 循环多次读取ADC值并累加
	for(t = 0; t < times; t++)
	{
		temp_val += Get_Adc(ch);  // 累加每次读取的ADC值
		Delay_ms(5);  // 延时5毫秒，确保每次读取之间有足够的时间间隔
	}
	// 计算并返回平均值
	return temp_val / times;  // 返回ADC值的平均值
}



void Buzzer_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(WATER_RCC, ENABLE); //使能ADC1通道时钟                      
	GPIO_InitStructure.GPIO_Pin = BUZZER_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //模拟输入引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 输出速度
	GPIO_Init(WATER_PORT, &GPIO_InitStructure);     // 初始化 GPIOB
	GPIO_SetBits(WATER_PORT, BUZZER_IO);
}	

void Buzzer(void)
{
	if (GPIO_ReadOutputDataBit(WATER_PORT, BUZZER_IO) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(WATER_PORT, BUZZER_IO);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(WATER_PORT, BUZZER_IO);					//则设置PA1引脚为低电平
	}
//	GPIO_ResetBits(WATER_PORT, BUZZER_IO);
//	Delay_ms(200); 
//	GPIO_SetBits(WATER_PORT, BUZZER_IO);
//	Delay_ms(200); 
//	GPIO_ResetBits(WATER_PORT, BUZZER_IO);
//	Delay_ms(200); 
//	GPIO_SetBits(WATER_PORT, BUZZER_IO);

}

uint8_t water_leavel_Check(uint8_t current_water_leavel)
{
	if (current_water_leavel <= min_water_leavel) 
	{
        return 1;
    }  
	else 
	{
        return 0;
    }
}
void set_min_water(uint8_t current_water_leavel_min)
{
	min_water_leavel=current_water_leavel_min;
}
