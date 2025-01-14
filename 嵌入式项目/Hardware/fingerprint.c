#include "fingerprint.h"
#include "delay.h"
#include "OLED.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
char RxPacket[100];				
uint8_t RxFlag;					

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);		

  // 清除发送完成标志
	//USART_ClearFlag(USART1, USART_FLAG_TC);     
}

/*****************  发送一个字符 **********************/
void Usart_SendByte(uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(DEBUG_USARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个二进制数 **********************/
void Usart_SendHalfWord(uint8_t * ch)
{
	for(size_t i=0;i<7;i++)
	{
		Usart_SendByte(ch[i]);	
	}
}

uint8_t calculateChecksum(uint8_t *data, size_t length) {
    uint8_t CheckXOR = 0x00;
    for (size_t i=1; i < length; i++) {
        CheckXOR ^= data[i];
    }
    return CheckXOR;
}

uint8_t GetRxFlag(void)
{
	if (RxFlag == 1)			
	{
		RxFlag = 0;
		return 1;					
	}
	return 0;						
}

void Get_AllUsers(void)
{
	Usart_SendHalfWord(ALL_USERS);
	Delay_ms(500);
	while (GetRxFlag() == 1)	
	{
		uint8_t commandLength = RxPacket[0];
		uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
		uint8_t role = RxPacket[3]; 
		//uint8_t status = RxPacket[4]; 
//		uint8_t checksum = RxPacket[5]; 
		if (commandLength == 0x09) 
		{
			if(role == ACK_SUCCESS)
			{
				OLED_ShowString(4,1,"AllUserNum:");
				OLED_ShowNum(4, 12, userID,2); 
			}
			else
			{
				OLED_ShowString(4, 1, "ERROR          "); 
			}
		} 
		else
		{
			OLED_ShowString(4, 1, "ERROR          "); 
		}
	}
}	


void match_fingerprint(void)
{
	Usart_SendHalfWord(MATCH);
	Delay_ms(500);
	if (GetRxFlag() == 1)	
	{
		uint8_t commandLength = RxPacket[0];
		uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
		uint8_t role = RxPacket[3]; 
		if (commandLength == 0x0c) 
		{
			
			if (userID == 0x0000) 	
			{
				OLED_ShowString(4, 1, "ERROR          "); 
			}
	        else	
			{
				if(role == 0X01)
				{
					OLED_ShowString(4,1,"User:");
					OLED_ShowNum(4, 6, userID,2); 
					OLED_ShowString(4,8,"        ");
					UserFlag=1;
					printf("UserFlag:1");
				}
				else
				{
					OLED_ShowString(4, 1, "ERROR          "); 
				}
				
			}
		} 
		else
		{
			OLED_ShowString(4, 1, "ERROR          "); 
		}
	}
}	

void REGISTER_USER(void)
{
	static unsigned char t=0;
	unsigned char OK_flag=0;
	if(t==0)
	{
		Usart_SendHalfWord(REG_USER1);
		Delay_ms(1000);
		
		if (GetRxFlag() == 1)	
		{
			uint8_t commandLength = RxPacket[0];
			uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
			uint8_t role = RxPacket[3]; 
			//uint8_t status = RxPacket[4]; 
			//uint8_t checksum = RxPacket[5]; 
			if (commandLength == 0x02&&userID == 0x0000&&role == ACK_SUCCESS) 
			{
				OLED_ShowString(3, 8, "33.3%"); 
				OK_flag=1;
			} 
			else
			{
				OLED_ShowString(3, 8, "     "); 
				OLED_ShowString(4, 1, "ERROR          "); 
			}
		}
	//	Delay_ms(100);
		if(OK_flag)
		{
			OK_flag=0;
			Usart_SendHalfWord(REG_USER2);
			Delay_ms(1000);
			if (GetRxFlag() == 1)	
			{
				uint8_t commandLength = RxPacket[0];
				uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
				uint8_t role = RxPacket[3]; 
				//uint8_t status = RxPacket[4]; 
				//uint8_t checksum = RxPacket[5]; 
				if (commandLength == 0x03&&userID == 0x0000&&role == ACK_SUCCESS) 
				{
					OLED_ShowString(3, 8, "66.6%"); 
					t=1;
					OLED_ShowString(4, 1, "Touch again!"); 
				} 
				else
				{
					OLED_ShowString(3, 8, "     "); 
					OLED_ShowString(4, 1, "ERROR          "); 
				}
			}
		}
	}
	else if(t==1)
	{
		Usart_SendHalfWord(REG_USER1);
        //Delay_ms(1000);		
		while(!GetRxFlag());
		if (GetRxFlag() == 1)	
		{
			uint8_t commandLength1 = RxPacket[0];
			uint16_t userID1 = (RxPacket[1] << 8) | RxPacket[2]; 
			uint8_t role1 = RxPacket[3]; 
			//uint8_t status = RxPacket[4]; 
			//uint8_t checksum = RxPacket[5]; 
			if (commandLength1 == 0x03 && role1 == ACK_SUCCESS) 
			{
				OLED_ShowString(3, 8, "OK!   "); 
				OLED_ShowString(4,1,"NewUser:");
				OLED_ShowNum(4, 9, userID1,2);
				OLED_ShowString(4,11,"     ");
				t=0;
			}
			else
			{
				OLED_ShowString(4, 1, "ERROR          "); 
				t=0;
			}
		}
	}
	
	
}

void DELECT_ALLUSERS(void)
{
    Usart_SendHalfWord(DEL_ALLUSER);
	Delay_ms(500);
	while (GetRxFlag() == 1)	
	{
		uint8_t commandLength = RxPacket[0];
		//uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
		uint8_t role = RxPacket[3]; 
		//uint8_t status = RxPacket[4]; 
//		uint8_t checksum = RxPacket[5]; 

		if (commandLength == 0x05) 
		{
			if(role == ACK_SUCCESS)
			{
				OLED_ShowString(4,1,"OK           ");
			}
			else 
			{
				OLED_ShowString(4, 1, "ERROR          "); 
			}
		} 
		else
		{
			OLED_ShowString(4, 1, "ERROR          "); 
		}
	}
}

void DELECT_ONEUSER(uint8_t UserNum) {
    uint8_t DEL_ONEUSER[] = {0xF5, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0xF5};
    DEL_ONEUSER[3] = UserNum;
    uint8_t chk = calculateChecksum(DEL_ONEUSER, 6);
    DEL_ONEUSER[6] = chk;
    Usart_SendHalfWord(DEL_ONEUSER);
	Delay_ms(500);
	while (GetRxFlag() == 1)	
	{
		uint8_t commandLength = RxPacket[0];
		//uint16_t userID = (RxPacket[1] << 8) | RxPacket[2]; 
		uint8_t role = RxPacket[3]; 
		//uint8_t status = RxPacket[4]; 
//		uint8_t checksum = RxPacket[5]; 

		if (commandLength == 0x04) 
		{
			if(role == ACK_SUCCESS)
			{
				OLED_ShowString(4,1,"OK           ");
			}
			else if(role == ACK_NOUSER)
			{
				OLED_ShowString(4, 1, "Unkonw User!   "); 
			}
		} 
		else
		{
			OLED_ShowString(4, 1, "ERROR          "); 
		}
	}
}


void DEBUG_USART_IRQHandler(void)
{
	static uint8_t Rxlocation = 0; 
    static uint8_t RxState = 0;		
	if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) == SET) 
	{
		uint8_t RxData = USART_ReceiveData(DEBUG_USARTx); 

		if(RxState == 0)//包头
		{
			if (RxData == 0xF5)			
			{
				RxState = 1;			
				Rxlocation = 0;			
			}
		}
		else if(RxState == 1)//数据
		{
			RxPacket[Rxlocation] = RxData;	
			Rxlocation ++;				
			if (Rxlocation >= 6)			
			{
				RxState = 2;			
			}
		}
		else if (RxState == 2)//包尾
		{
			if (RxData == 0xF5)			
			{
				RxState = 0;			
				RxFlag = 1;	
            }				
		}
		USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RXNE); 
	}
}



