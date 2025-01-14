#include "dht11.h"
#include "Delay.h"

uint8_t max_temperature= 32;
uint8_t min_humidity= 50;

uint8_t current_temperature;
uint8_t current_humidity;

void DHT11_IO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);	 
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	
 	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;			
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				
}

void DHT11_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    
	
 	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;				 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
 	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				 
}


void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	
	DHT11_DQ_OUT=0; 	
	Delay_ms(20);    	
	DHT11_DQ_OUT=1; 	
	Delay_us(30);     	
}

u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();	 
    while (DHT11_DQ_IN&&retry<100)
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

u8 DHT11_Read_Byte(void)    
{        
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
  }						    
  return dat;
}

u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
			current_humidity = buf[0];
			current_temperature = buf[2];
		}
	}else return 1;
	return 0;	    
}


u8 DHT11_Init(void)
{	   
	DHT11_Rst();  
	return DHT11_Check();
} 


uint8_t humidityandtemperature_Check(uint8_t current_temperature,uint8_t current_humidity)
{
	uint8_t t=0;
	if (current_humidity <= min_humidity) 
	{
       t++;
    } 
    if (current_temperature >= max_temperature) 
	{
       t++;
	}
	return t;
}
void set_min_humidity(uint8_t current_humidity_min)
{
	min_humidity=current_humidity_min;
}
	

void set_max_temperature(uint8_t max_temperature)
{
	max_temperature=max_temperature;
}
