#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   

////IO操作函数											   
#define	DHT11_DQ_OUT PAout(11) //数据端口	PA11 
#define	DHT11_DQ_IN  PAin(11)  //数据端口	PA11

#define DHT11_GPIO_PORT    	GPIOA			              //GPIO端口
#define DHT11_GPIO_CLK 	    RCC_APB2Periph_GPIOA		//GPIO端口时钟
#define DHT11_GPIO_PIN		GPIO_Pin_11			        //连接到SCL时钟线的GPIO

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    
uint8_t humidityandtemperature_Check(uint8_t current_temperature,uint8_t current_humidity);
void set_min_humidity(uint8_t current_humidity_min);
void set_max_temperature(uint8_t max_temperature);
#endif

