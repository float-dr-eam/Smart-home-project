#include "esp8266.h"
#include "common.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "delay.h"
#include "lcd.h"

extern u8 temperature;
extern u8 humidity;

volatile uint8_t ucTcpClosedFlag = 0;

char cStr [ 1500 ] = { 0 };
// static void                   ESP8266_GPIO_Config                 ( void );
static void                   ESP8266_USART_Config                ( void );
static void                   ESP8266_USART_NVIC_Configuration    ( void );
struct STRUCT_USARTx_Fram     strEsp8266_Fram_Record = { 0 };
static char User[]="AT+MQTTUSERCFG=0,1,\"NULL\",\"ESP_01S&k1st6SO97yW\",\"fa14fbe5481d125d7d152ebbf34e4e95a94de86669e182bbb380eef7859f3d10\",0,0,\"\"";
static char rst_data[]="AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"currentTemperature\\\":11.1\\,\\\"currentHumidity\\\":77.7\\,\\\"currentLightswitch\\\":22.2}\",1,0";

// 函数用于生成 MQTT 发布命令字符串
void create_mqtt_all_command(u8 temperature, u8 humidity,u8 waterlevel,u8 warningflag, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"currentTemperature\\\":%d\\,\\\"currentHumidity\\\":%d\\,\\\"currentwaterlevel\\\":%d\\,\\\"alertflag\\\":%d}\",1,0",
             temperature, humidity,waterlevel,warningflag);
}
// 函数用于生成 MQTT 发布命令字符串
void create_mqtt_pub_command(u8 temperature, u8 humidity, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"currentTemperature\\\":%d\\,\\\"currentHumidity\\\":%d}\",1,0",
             temperature, humidity);
}
// 函数用于生成 MQTT 发布命令字符串，只包含 currentwaterlevel
void create_mqtt_waterlevel_command(u8 waterlevel, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"currentwaterlevel\\\":%d}\",1,0",
             waterlevel);
}
// 函数用于生成 MQTT 发布命令字符串，只包含 currentwaterlevel
void create_mqtt_jsqswitch_command(bool jsqswitch, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"jiashiflag\\\":%d}\",1,0",
             jsqswitch);
}
// 函数用于生成 MQTT 发布命令字符串，只包含 currentwaterlevel
void create_mqtt_ledswitch_command(bool ledswitch, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"ledflag\\\":%d}\",1,0",
             ledswitch);
}
// 函数用于生成 MQTT 发布命令字符串，只包含 currentwaterlevel
void create_mqtt_warningflag_command(u8 warningflag, char data[], size_t len) 
{
    // 确保有足够空间存放格式化的字符串
    snprintf(data, len,
             "AT+MQTTPUB=0,\"/k1st6SO97yW/ESP_01S/user/ESP_01S\",\"{\\\"alertflag\\\":%d}\",1,0",
             warningflag);
}
void extract_data(char *json) //解析天气数据
{    
	char empty_string1[4]="    "; 
	char empty_string2[4]="    "; 
    char location[50] = "";
    char weather[30] = "";
    char date[11] = "";
    uint8_t len =0;
    // 提取地点
    sscanf(strstr(json, "\"name\":\"") + 8, "%49[^\"]", location);   
    // 提取天气
    sscanf(strstr(json, "\"text\":\"") + 8, "%29[^\"]", weather);
    // 提取日期（只提取到 "YYYY-MM-DD"）
    sscanf(strstr(json, "last_update\":\"") + 14, "%10s", date); // 只读取10个字符

//    // 打印结果
//    printf("地点: %s\n", location);
//    printf("天气: %s\n", weather);
//    printf("时间: %s\n", date);
   
	LCD_ShowString(70,48,(u8 *)date,RED,LIGHTBLUE,24,0); //日期
	
	len = strlen(location);// 填充空字符串
	memset(empty_string1, ' ', sizeof(empty_string2) - 1);
	empty_string1[sizeof(empty_string1) - 1] = '\0';
	LCD_ShowChinese24x24(70,80,(u8 *)location,RED,LIGHTBLUE,24,0);//地点
    LCD_ShowString(70+(len*24/3),80,(u8 *)empty_string1,RED,LIGHTBLUE,24,0);
	
    len = strlen(weather);// 填充空字符串
    memset(empty_string2, ' ', sizeof(empty_string2) - 1);
    empty_string2[sizeof(empty_string2) - 1] = '\0';
	LCD_ShowChinese24x24(70,112,(u8 *)weather,RED,LIGHTBLUE,24,0);//天气
	LCD_ShowString(70+(len*24/3),112,(u8 *)empty_string2,RED,LIGHTBLUE,24,0);
    // 清除原始json缓冲区
    memset(json, 0, RX_BUF_MAX_LEN);
}

void extract_mqtt_flags(char *mqtt_msg,bool* jsq_switch,bool* LED_switch) 
{
    char *json = strstr(mqtt_msg, "+MQTTSUBRECV:0");
	
    if (json == NULL) 
	{
        return;
    }
    else
	{
		// 提取 jiashiflag (假设值是 "true" 或 "false")
		if (strstr(mqtt_msg, "\"jiashiflag\":1")) 
		{
			*jsq_switch = true;
		} 
		if (strstr(mqtt_msg, "\"jiashiflag\":0") > strstr(mqtt_msg, "\"jiashiflag\":1")) 
		{
			*jsq_switch = false;
		}
		// 提取 ledflag (假设值是 "true" 或 "false")
		if (strstr(mqtt_msg, "\"ledflag\":1")) 
		{
			*LED_switch = true;
		} 
		if (strstr(mqtt_msg, "\"ledflag\":0") > strstr(mqtt_msg, "\"ledflag\":1")) 
		{
			*LED_switch = false;
		}
        memset(mqtt_msg, 0, RX_BUF_MAX_LEN);
		strEsp8266_Fram_Record .InfBit .FramLength = 0; //初始化帧长度为0
	}
	// 清除原始json缓冲区
    
}

void ESP8266_weather ( void )
{
		//TCP连接
	ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80", "OK", NULL, 1000);
	ESP8266_Cmd("AT+CIPMODE=1","OK",NULL,1000);//透传模式
	Delay_ms(500);
	ESP8266_Cmd("AT+CIPSEND", ">",NULL,1000);//发送数据
	Delay_ms(500);
	ESP8266_Cmd("GET https://api.seniverse.com/v3/weather/now.json?key=S3SieXzcBJfTtnfaM&location=shenzhen&language=zh-Hans&unit=c", NULL, NULL, 2000);
    Delay_ms(1000);
	extract_data(strEsp8266_Fram_Record .Data_RX_BUF);
	Delay_ms(500);
	macESP8266_Usart ( "%s", "+++" );
	Delay_ms(500);
	ESP8266_Cmd("+++","ERROR",NULL,100);
	ESP8266_Cmd("AT+CIPMODE=0","CLOSED",NULL,100);
    ESP8266_Cmd("AT+CIPCLOSE","OK",NULL,100);
	Delay_ms(2000);
}

void ESP8266_Aliyun(char * data)
{

    // 发布消息
    ESP8266_Cmd(data, "OK", NULL, 800);

//    // 可选: 关闭 MQTT 连接
//    ESP8266_Cmd("AT+MQTTCLOSE=0", "OK", NULL, 1000);	
}



void ESP8266_StaTcpClient ( void )
{
	ESP8266_AT_Test ();
	ESP8266_Rst();
	ESP8266_Net_Mode_Choose ( STA );
    while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
	ESP8266_Cmd("AT+CIPSNTPCFG=1,8", "OK", NULL, 500);
	ESP8266_weather ();
    // MQTT用户配置
    ESP8266_Cmd( User, "OK", NULL, 2000);
    Delay_ms(500);
	//连接客户端
	ESP8266_Cmd( "AT+MQTTCLIENTID=0,\"k1st6SO97yW.ESP_01S|securemode=2\\,signmethod=hmacsha256\\,timestamp=1733734784874|\"", "OK", NULL, 2000);
    // 连接到阿里云 MQTT
    ESP8266_Cmd("AT+MQTTCONN=0,\"iot-06z00gqdozwvyii.mqtt.iothub.aliyuncs.com\",1883,1", "OK", NULL, 5000);
    Delay_ms(1000);
    // 订阅主题
    ESP8266_Cmd("AT+MQTTSUB=0,\"/sys/k1st6SO97yW/ESP_01S/thing/service/property/set\",1", "OK", NULL, 2000);
    Delay_ms(1000);
	ESP8266_Aliyun(rst_data);
}


// @brief  初始化 ESP8266的RST，EN的GPIO 低功耗

static void ESP8266_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//macESP8266_CH_PD_APBxClock_FUN ( macESP8266_CH_PD_CLK, ENABLE ); 
											   
	//GPIO_InitStructure.GPIO_Pin = macESP8266_CH_PD_PIN;	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	//GPIO_Init ( macESP8266_CH_PD_PORT, & GPIO_InitStructure );	 

	
	macESP8266_RST_APBxClock_FUN ( macESP8266_RST_CLK, ENABLE ); //复位引脚初始化
											   
	GPIO_InitStructure.GPIO_Pin = macESP8266_RST_PIN;	

	GPIO_Init ( macESP8266_RST_PORT, & GPIO_InitStructure );	 


}
 // @brief  ESP8266初始化函数
void ESP8266_Init ( void )
{
	ESP8266_GPIO_Config (); 	
	ESP8266_USART_Config (); 
//	macESP8266_RST_HIGH_LEVEL();
//	macESP8266_CH_DISABLE();
}

// @brief  ESP8266的 USART初始化

static void ESP8266_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* config USART clock */
	macESP8266_USART_APBxClock_FUN ( macESP8266_USART_CLK, ENABLE );
	macESP8266_USART_GPIO_APBxClock_FUN ( macESP8266_USART_GPIO_CLK, ENABLE );
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macESP8266_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macESP8266_USART_TX_PORT, &GPIO_InitStructure);  
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = macESP8266_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macESP8266_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macESP8266_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macESP8266_USARTx, &USART_InitStructure);
	
	
	/* 中断*/
	USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //rx的开始
	USART_ITConfig ( macESP8266_USARTx, USART_IT_IDLE, ENABLE ); //rx的结束
	ESP8266_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macESP8266_USARTx, ENABLE);
	
	
}


//ESP8266 USART的 NVIC 

static void ESP8266_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig ( macNVIC_PriorityGroup_x );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macESP8266_USART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



// ESP8266_Rst

void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 500 );   	
	
	#else
	 macESP8266_RST_LOW_LEVEL();
	 Delay_ms( 500 ); 
	 macESP8266_RST_HIGH_LEVEL();
	#endif

}


/*
 ESP8266_Cmd
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0; //初始化帧长度为0
	macESP8266_Usart ( "%s\r\n", cmd ); // 发送命令到 ESP8266

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) ) //如果没有回复参数
		return true; //直接返回真
	
	Delay_ms( waittime ); //等待指定的时间
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0'; // 在接收缓冲区末尾添加字符串结束符
	//调试
	//macPC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF ); // 输出接收到的数据到 PC 串口

	if ( ( reply1 != 0 ) && ( reply2 != 0 ) ) // 如果有两个回复参数
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); // 检查是否包含任一回复内容
	
	else if ( reply1 != 0 ) // 如果只有第一个回复参数
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) ); // 检查是否包含第一个回复内容
	
	else // 如果只有第二个回复参数
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); // 检查是否包含第二个回复内容	
}



/*
AT 测试
 */

void ESP8266_AT_Test ( void )
{
	char count=0;
	
	macESP8266_RST_HIGH_LEVEL();	
	Delay_ms( 10 );
	while ( count < 10 )
	{
		if( ESP8266_Cmd ( "AT", "OK", NULL, 100 ) ) return;
		ESP8266_Rst();
		++ count;
	}
}


/*
ESP8266_Net_Mode_Choose
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}


/*
ESP8266_JoinAP  连接WiFi
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 5000 );
	
}


/*
ESP8266_BuildAP  创建WiFi热点
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}


/*
ESP8266_Enable_MultipleId  模块多连接
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	
	return ESP8266_Cmd ( "AT+CIPMUX=%d", "OK", 0, 500 );
	
}


/*
 * 函数名称：ESP8266_Link_Server
 * 功能     : WF-ESP8266连接服务器的处理程序
 * 参数     : enumE，表示状态控制
 *           ip，服务器IP地址字符串
 *           ComNum，连接的序号
 *           id，服务器连接的ID
 * 返回值   : 1，连接成功
 *           0，连接失败
 * 实现     : 具体实现方式
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );
	
}


/*
* ESP8266_StartOrShutServer  开启或关闭服务器
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
ESP8266_Get_LinkStatus  连接状态

 */
uint8_t ESP8266_Get_LinkStatus ( void )
{
	if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:2\r\n" ) )
			return 2;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:3\r\n" ) )
			return 3;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:4\r\n" ) )
			return 4;		

	}
	
	return 0;
	
}



// ESP8266_Get_IdLinkStatus ID的连接状态

uint8_t ESP8266_Get_IdLinkStatus ( void )
{
	uint8_t ucIdLinkStatus = 0x00;
	
	
	if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:0," ) )
			ucIdLinkStatus |= 0x01;
		else 
			ucIdLinkStatus &= ~ 0x01;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:1," ) )
			ucIdLinkStatus |= 0x02;
		else 
			ucIdLinkStatus &= ~ 0x02;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:2," ) )
			ucIdLinkStatus |= 0x04;
		else 
			ucIdLinkStatus &= ~ 0x04;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:3," ) )
			ucIdLinkStatus |= 0x08;
		else 
			ucIdLinkStatus &= ~ 0x08;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:4," ) )
			ucIdLinkStatus |= 0x10;
		else 
			ucIdLinkStatus &= ~ 0x10;	

	}
	
	return ucIdLinkStatus;
	
}


/*
 * 函数名称：ESP8266_Inquire_ApIp
 * 函数功能：获取 F-ESP8266 的 AP IP
 * 输入参数：pApIp，指向存储 AP IP 的变量的指针
 *           ucArrayLength，表示 AP IP 数据的最大长度
 * 输出结果：0，获取成功
 *           1，获取失败
 */

uint8_t ESP8266_Inquire_ApIp ( char * pApIp, uint8_t ucArrayLength )
{
	char uc;
	
	char * pCh;
	
	
  ESP8266_Cmd ( "AT+CIFSR", "OK", 0, 500 );
	
	pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "APIP,\"" );
	
	if ( pCh )
		pCh += 6;
	
	else
		return 0;
	
	for ( uc = 0; uc < ucArrayLength; uc ++ )
	{
		pApIp [ uc ] = * ( pCh + uc);
		
		if ( pApIp [ uc ] == '\"' )
		{
			pApIp [ uc ] = '\0';
			break;
		}
		
	}
	
	return 1;
	
}







/*
 ESP8266_ExitUnvarnishSend  退出透传模式

 */
void ESP8266_ExitUnvarnishSend ( void )
{
	Delay_ms( 1000 );
	
	macESP8266_Usart ( "+++" );
	
	Delay_ms( 500 ); 
	
}


/*
 * 函数名称：ESP8266_SendString
 * 函数功能：WF-ESP8266进行字符串发送
 * 输入参数：enumEnUnvarnishTx，枚举值表示是否开启缓存发送
 *           pStr，待发送的字符串
 *           ulStrLength，待发送字符串的长度
 *           ucId，标识ID，用于发送字符串的标识
 * 输出结果：1，发送成功
 *           0，发送失败
 */

bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		macESP8266_Usart ( "%s", pStr );
		
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


/*
 * 函数名称：ESP8266_ReceiveString
 * 函数功能：WF-ESP8266进行字符串接收
 * 输入参数：enumEnUnvarnishTx，枚举值表示是否开启缓存接收
 * 输出结果：接收到的字符串的长度
 */

char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if ( enumEnUnvarnishTx )
		pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;
	
	else 
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
	
}
void macESP8266_USART_INT_FUN(void) // USART中断处理函数，处理ESP8266的数据接收
{	
    uint8_t ucCh; // 定义一个变量存储接收到的数据

    // 检查接收中断是否被触发
    if (USART_GetITStatus(macESP8266_USARTx, USART_IT_RXNE) != RESET)
    {
        // 从USART接收数据
        ucCh = USART_ReceiveData(macESP8266_USARTx);
		
        // 如果当前帧长度小于最大缓冲区长度-1，则存储接收到的数据
        if (strEsp8266_Fram_Record.InfBit.FramLength < (RX_BUF_MAX_LEN - 1))
            strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++] = ucCh;
    }
    
    // 检查空闲中断，表示数据接收完毕
    if (USART_GetITStatus(macESP8266_USARTx, USART_IT_IDLE) == SET) 
    {
        // 设置帧完成标志
        strEsp8266_Fram_Record.InfBit.FramFinishFlag = 1;

        // 读取数据以清除中断状态
        ucCh = USART_ReceiveData(macESP8266_USARTx); // 读取数据以避免丢失（清除USART_SR寄存器）

        // 检查接收到的数据是否包含 "CLOSED\r\n"
        ucTcpClosedFlag = strstr(strEsp8266_Fram_Record.Data_RX_BUF, "CLOSED\r\n") ? 1 : 0;
    }	

    // 可以发送其他数据（注释掉的代码）
    // Usart_SendString(USART1, q);
    
    //将最后接收到的字符发送到USART1（调试）
   // USART_SendData(USART1, ucCh);
}




