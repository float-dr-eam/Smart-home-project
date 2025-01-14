#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "string.h"
#include "lcd.h"
#include "pic.h"
#include "esp8266.h"
#include "Timer.h"
#include "TIM3.h"
#include "Key.h"
#include "fingerprint.h"
#include "PWM.h"
#include "water.h"
#include "DHT11.h"
#include "relay.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


//extern uint8_t last_signal;

unsigned char hanzi6[]="智能家居系统";

unsigned char hanzi0[]="日期";
unsigned char time[]="2024-09-26";

unsigned char hanzi1[]="地点";
unsigned char place[]="地点";

unsigned char hanzi2[]="天气";
unsigned char weather[]="天气";

unsigned char hanzi3[]="温度";
unsigned char temp[]="25";

unsigned char hanzi4[]="湿度";
unsigned char hum[]="25";

unsigned char hanzi5[]="水位";
unsigned char water[]="25";


//按键
u8 KeyNum = 0;
u8 KeyFlag = 0;
u8 UserNum = 0;
bool UserFlag=0;

//温、湿度
u8 temperature= 66;
u8 humidity= 66;
bool reflag=0;//温、湿度、水位更新标志位
//水位
u8 waterlevel=0;
double water_data= 66;

//加湿器、灯的开关
bool jsq_switch= 0;
bool LED_switch= 0;

//更新MQTT标志位
u8 update_flag=0;
u8 Num=0;//水位
char data[255];//MQTT数据包
//报警标志位
u8 warning_leavel=0;
u8 warning_reflag=0;//更新报警标志位
u8 get_number_length_sprintf(u8 number);
void Key_show(void);
void TW_show(void);
int main(void)
{

	OLED_Init();		//OLED初始化
	LED_Init();			//LED 初始化
	Serial_Init();		//语音串口1初始化
	LCD_Init();          //LCD屏幕初始化
	OLED_ShowString(2, 1, "SYSTEM INITING..");
	LCD_Fill(0,0,LCD_W,LCD_H,LIGHTBLUE);
	LCD_ShowPicture(140,140,100,100,gImage_PIC);//显示图片
	LCD_ShowChinese32x32(24,8,hanzi6,BLACK,LIGHTBLUE,32,1);//标题
	
	LCD_ShowChinese24x24(10,48,hanzi0,BLACK,LIGHTBLUE,24,1);//日期+40
	LCD_ShowChar(58,48,':',BLACK,WHITE,24,1);	
	LCD_ShowString(70,48,time,RED,LIGHTBLUE,24,0);
	
	LCD_ShowChinese24x24(10,80,hanzi1,BLACK,LIGHTBLUE,24,1);//地点+32
	LCD_ShowChar(58,80,':',BLACK,WHITE,24,1);	
	LCD_ShowChinese24x24(70,80,place,RED,LIGHTBLUE,24,1);
	
	LCD_ShowChinese24x24(10,112,hanzi2,BLACK,LIGHTBLUE,24,1);//天气
	LCD_ShowChar(58,112,':',BLACK,WHITE,24,1);
	LCD_ShowChinese24x24(70,112,weather,RED,LIGHTBLUE,24,0);
	
	LCD_ShowChinese24x24(10,144,hanzi3,BLACK,LIGHTBLUE,24,1);//温度
	LCD_ShowChar(58,144,':',BLACK,WHITE,24,1);
	LCD_ShowString(70,144,temp,RED,LIGHTBLUE,24,0);
	LCD_ShowChinese24x24(94,144,(u8*)"℃",RED,LIGHTBLUE,24,1);
		
	LCD_ShowChinese24x24(10,176,hanzi4,BLACK,BLACK,24,1);//湿度
	LCD_ShowChar(58,176,':',BLACK,WHITE,24,1);
	LCD_ShowString(70,176,hum,RED,LIGHTBLUE,24,0);
	LCD_ShowChar(94,176,(u8)'%',RED,LIGHTBLUE,24,1);
	
	LCD_ShowChinese24x24(10,208,hanzi5,BLACK,BLACK,24,1);//水位
	LCD_ShowChar(58,208,':',BLACK,WHITE,24,1);
	LCD_ShowString(70,208,water,RED,LIGHTBLUE,24,0);
	LCD_ShowChar(94,208,(u8)'%',RED,LIGHTBLUE,24,1);
	Key_Init();//按键初始化
	
    ESP8266_Init (); // 串口2初始化  
    ESP8266_StaTcpClient ();//esp8266初始化 
	USART_Config();//指纹串口初始化
	Adc_Init();//水位ADC初始化
    Buzzer_Init(); //蜂鸣器初始化
	Relay_Init();//继电器初始化
    DHT11_Init();//温湿度传感器初始化
	PWM_Init();//加湿器PWM初始化
	OLED_Clear();//清屏
	OLED_ShowString(1, 1, "FingerPrint SYS");//进入指纹识别系统
	Timer_Init();//1s定时器初始化
	TIM3_Init();
	
	create_mqtt_jsqswitch_command(jsq_switch,data, sizeof(data));
	ESP8266_Aliyun(data);
	create_mqtt_ledswitch_command(LED_switch,data, sizeof(data));
	ESP8266_Aliyun(data);
	create_mqtt_warningflag_command(warning_leavel,data,  sizeof(data)) ;
	ESP8266_Aliyun(data);
	while (1)
	{
       // KeyNum =Key_GetNum();
		Key_show();
        TW_show();
		if(reflag)
		{
			reflag=!reflag;
			DHT11_Read_Data(&temperature, &humidity);//读温、湿度
			LCD_ShowIntNum(70,144,temperature,2,RED,LIGHTBLUE,24); //更新温湿度
			LCD_ShowChinese24x24(70+get_number_length_sprintf(temperature)*12,144,(u8*)"℃",RED,LIGHTBLUE,24,0);//94
			LCD_ShowIntNum(70,176,humidity,2,RED,LIGHTBLUE,24);
			LCD_ShowString(70+get_number_length_sprintf(humidity)*12,176,(u8*)"% ",RED,LIGHTBLUE,24,0);//94
			warning_leavel=humidityandtemperature_Check(temperature,humidity);
            //读水位
			water_data = Get_Adc_Average(8,4);
			water_data = (double)water_data / 4096 * 100;//更新水位
			waterlevel = (u8)water_data;
			LCD_ShowIntNum(70,208,waterlevel,2,RED,LIGHTBLUE,24);
			LCD_ShowString(70+get_number_length_sprintf(waterlevel)*12,208,(u8*)"% ",RED,LIGHTBLUE,24,0);//94
			warning_leavel = warning_leavel+water_leavel_Check(waterlevel);
		}
	}

}






void Key_show(void)
{
	if(KeyNum==1)
	{
		KeyNum = 0;
		KeyFlag = (KeyFlag+1) % 7;
		if(KeyFlag==0)//系统界面
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "FingerPrint SYS");
		}
		else if(KeyFlag==1)//匹配
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "MatchFingerPrint");
			OLED_ShowString(3, 1, "Status:");
		}
		else if(KeyFlag==2)//用户数
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "Get ALL UsersNum");
			OLED_ShowString(3, 1, "Status:");
		}
		else if(KeyFlag==3)//退出
		{			
			OLED_Clear();
			OLED_ShowString(1, 1, "     Log Out    ");
			OLED_ShowString(3, 1, "Status:");
			
		}
		else if(KeyFlag==4)//注册
		{	            
			OLED_Clear();
			OLED_ShowString(1, 1, "Register NewUser");
			OLED_ShowString(3, 1, "Status:");	
	
		}
		else if(KeyFlag==5)//删除一个用户
		{
			OLED_Clear();
			OLED_ShowString(1, 2, "Delect OneUser");
			OLED_ShowString(3, 1, "Status:");
			OLED_ShowString(2, 1, "UserID:");
			OLED_ShowNum(2,8,UserNum,2);
		}
		else if(KeyFlag==6)//删除所有用户
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "Delect ALL Users");
			OLED_ShowString(3, 1, "Status:");
		}
	}
	
	if(KeyNum==2 && KeyFlag==0)
	{
		KeyNum = 0;
	}
	else if(KeyNum==2 && KeyFlag==1)//匹配
	{
		KeyNum = 0;
		OLED_ShowString(4, 1, "               ");
		match_fingerprint();
	}
	else if(KeyNum==2 && KeyFlag==2)//用户数
	{
		KeyNum = 0;
		OLED_ShowString(4, 1, "               "); 
		Get_AllUsers();
		
	}
	else if(KeyNum==2 && KeyFlag==3)//退出
	{
		KeyNum = 0;
		UserFlag=0;
		printf("UserFlag:0");
		OLED_ShowString(4, 1, "OK!            "); 
	}
	else if(KeyNum==2 && KeyFlag==4&&UserFlag==1)//注册
	{
		KeyNum = 0;
		OLED_ShowString(4, 1, "               "); 
		REGISTER_USER();
	}
	else if(KeyNum==2 && KeyFlag==5&&UserFlag==1)//删一个用户
	{
		KeyNum = 0;
        OLED_ShowString(4, 1, "               "); 
		DELECT_ONEUSER(UserNum);
	}
	else if(KeyNum==2 && KeyFlag==6&&UserFlag==1)//删所有用户
	{
		KeyNum = 0;
		OLED_ShowString(4, 1, "               "); 
		DELECT_ALLUSERS();
	}
	else if(KeyNum==2 && KeyFlag!=0&& KeyFlag!=1&& KeyFlag!=2&& KeyFlag!=3&&UserFlag==0) //其余情况
	{
		KeyNum = 0;
		OLED_ShowString(4, 1, "No Access!     "); 
	}
	
	if(KeyNum==3 && KeyFlag==5)
	{
		KeyNum = 0;
		UserNum++;
		OLED_ShowString(2, 1, "UserID:");
		OLED_ShowNum(2,8,UserNum,2);
	}
	if(KeyNum==4 && KeyFlag==5)
	{
		KeyNum = 0;
		UserNum--;
		OLED_ShowString(2, 1, "UserID:");
		OLED_ShowNum(2,8,UserNum,2);
	}
}
void TW_show(void)//天文51的字符串处理
{
	if (Serial_RxFlag == 1) // 如果接收到数据标志为1
	{
		/* 处理接收到的指令 */
		if (strcmp(Serial_RxPacket, "led_on") == 0) // 如果接收到的指令是led_on
		{
			relay_on1(); // 打开灯光
			LED_switch=1;
			create_mqtt_ledswitch_command(LED_switch,data, sizeof(data));
			ESP8266_Aliyun(data);
		}
		else if (strcmp(Serial_RxPacket, "led_off") == 0) // 如果接收到的指令是led_off
		{
			relay_off1(); // 关闭灯光
		    LED_switch=0;
			create_mqtt_ledswitch_command(LED_switch,data, sizeof(data));
			ESP8266_Aliyun(data);
		}
		else if (strcmp(Serial_RxPacket, "hum_on") == 0) // 如果接收到的指令是hum_on
		{
			relay_on2(); // 打开加湿器
			jsq_switch=1;
			create_mqtt_jsqswitch_command(jsq_switch,data, sizeof(data));
			ESP8266_Aliyun(data);	
		}
		else if (strcmp(Serial_RxPacket, "hum_off") == 0) // 如果接收到的指令是hum_off
		{
			relay_off2(); // 关闭加湿器
			jsq_switch=0;
			create_mqtt_jsqswitch_command(jsq_switch,data, sizeof(data));
	        ESP8266_Aliyun(data);	
		}
		else if (strcmp(Serial_RxPacket, "update_tem") == 0) // 如果接收到的指令是update_tem
		{
			create_mqtt_pub_command(temperature, humidity, data, sizeof(data));
            ESP8266_Aliyun(data); // 更新温湿度
		}
		else if (strcmp(Serial_RxPacket, "update_water") == 0) // 如果接收到的指令是update_water
		{
			create_mqtt_waterlevel_command(waterlevel, data, sizeof(data));
            ESP8266_Aliyun(data); // 更新水位
		}
		else if (strcmp(Serial_RxPacket, "match_finger") == 0) // 如果接收到的指令是update_water
		{
			KeyNum=2;
			KeyFlag=1;
			OLED_Clear();
			OLED_ShowString(1, 1, "MatchFingerPrint");
			OLED_ShowString(3, 1, "Status:");
		}
		else if (strcmp(Serial_RxPacket, "all_user") == 0) // 如果接收到的指令是update_water
		{
			KeyNum=2;
			KeyFlag=2;
            OLED_Clear();
			OLED_ShowString(1, 1, "Get ALL UsersNum");
			OLED_ShowString(3, 1, "Status:");
		}
		else if (strcmp(Serial_RxPacket, "log_out") == 0)// 如果接收到的指令不在已知指令之内
		{
			KeyNum=2;
			KeyFlag=3;
			OLED_Clear();
			OLED_ShowString(1, 1, "     Log Out    ");
			OLED_ShowString(3, 1, "Status:");
		}
		Serial_RxFlag = 0; // 重置接收标志为0，准备接收下一条指令
	}
}

u8 get_number_length_sprintf(u8 number) //计算数字字符的长度
{
    char buffer[4]; // u8的最大值是255，所以最多需要4个字符空间（包括'\0'）
    sprintf(buffer, "%d", number);
    return strlen(buffer);
}

// 定时器 TIM1 更新中断服务函数
void TIM1_UP_IRQHandler(void)
{
    // 检查定时器 TIM1 的更新中断标志是否被设置
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        // 清除定时器的更新中断标志
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        ++update_flag;
        if (update_flag >= 15) // 假设每秒中断一次，这里表示 10 秒定时器
        {
            update_flag = 0;
            // 调用函数创建命令字符串
			create_mqtt_all_command(temperature,humidity,waterlevel,warning_leavel,data, sizeof(data));
			ESP8266_Aliyun(data);
        }
		if(!(update_flag % 5))//5s定时器
		{
			reflag = !reflag;
		}
        if(!(update_flag%2))//2s定时器0.25*8
		{
			warning_reflag= !warning_reflag;//报警
			if(warning_reflag)
			{
				if(warning_leavel>=2)
				{
					LED1_Turn();
					Buzzer();
				}
				else if(2>warning_leavel && warning_leavel>0)//2
				{
					LED1_Turn();
					GPIO_SetBits(WATER_PORT, BUZZER_IO);
					//Buzzer();
				}
				else//01
				{
					GPIO_SetBits(WATER_PORT, BUZZER_IO);
					GPIO_SetBits(GPIOA, GPIO_Pin_12);
				}
			}
		}

      
    }
}		

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
//		Num=(Num+1)%8;
		extract_mqtt_flags(strEsp8266_Fram_Record .Data_RX_BUF,&jsq_switch,&LED_switch);
		if(jsq_switch)
		{
			relay_on2(); // 打开加湿器
		}
		else
		{
			relay_off2(); // 关闭加湿器
		}
		if(LED_switch)
		{
			relay_on1(); // 打开灯光
		}
		else
		{
			relay_off1(); // 关闭灯光
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}


void EXTI15_10_IRQHandler(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)			//读PB12输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 1;												//置键码为1
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)			//读PB13输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 2;												//置键码为2
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)			//读PB14输入寄存器的状态，如果为0，则代表按键3按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 3;												//置键码为2
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)			//读PB15输入寄存器的状态，如果为0，则代表按键4按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 4;												//置键码为2
	}
	EXTI_ClearITPendingBit(EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15);
}

