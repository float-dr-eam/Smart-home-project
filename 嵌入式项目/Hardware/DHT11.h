#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   

////IO��������											   
#define	DHT11_DQ_OUT PAout(11) //���ݶ˿�	PA11 
#define	DHT11_DQ_IN  PAin(11)  //���ݶ˿�	PA11

#define DHT11_GPIO_PORT    	GPIOA			              //GPIO�˿�
#define DHT11_GPIO_CLK 	    RCC_APB2Periph_GPIOA		//GPIO�˿�ʱ��
#define DHT11_GPIO_PIN		GPIO_Pin_11			        //���ӵ�SCLʱ���ߵ�GPIO

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
uint8_t humidityandtemperature_Check(uint8_t current_temperature,uint8_t current_humidity);
void set_min_humidity(uint8_t current_humidity_min);
void set_max_temperature(uint8_t max_temperature);
#endif

