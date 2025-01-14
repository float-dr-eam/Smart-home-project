#ifndef __WATER_H
#define __WATER_H	
#include "sys.h"
 
#define WATER_IO   GPIO_Pin_0
#define BUZZER_IO  GPIO_Pin_1
#define WATER_PORT GPIOB
#define WATER_RCC  RCC_APB2Periph_GPIOB

void Adc_Init(void);
u16 Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
void Buzzer(void); 
void Buzzer_Init(void); 
uint8_t water_leavel_Check(uint8_t current_water_leavel);
void set_min_water(uint8_t current_water_leavel_min);
#endif 
