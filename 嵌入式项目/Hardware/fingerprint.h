#ifndef __FINGERPRINT_H
#define	__FINGERPRINT_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
//Q3结果
#define ACK_SUCCESS 0x00 //成功
#define ACK_FAIL 0x01   //失败
#define ACK_FULL 0x04    //库满
#define ACK_NOUSER 0x05   //无用户
#define ACK_USER_EXIST 0x07  //已存在
#define ACK_TIMEOUT 0x08    //超时
#define ACK_HARDWAREERROR 0x0A   //硬件错误
#define ACK_IMAGEERROR 0x10   //图像错误
#define ACK_BREAK 0x18       //终止
#define ACK_ALGORITHMFAIL 0x11  //攻击检测
#define ACK_HOMOLOGYFAIL 0x12   //校验错误
extern bool UserFlag;

static uint8_t ALL_USERS[] = {0xF5, 0x09, 0x00, 0x00, 0x00, 0x00, 0x09, 0xF5};

static uint8_t REG_USER1[] = {0xF5, 0x02, 0x00, 0x00, 0x01, 0x00, 0x03, 0xF5};
static uint8_t REG_USER2[] = {0xF5, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF5};

//uint8_t SUCCESS0[]   = {0xF5, 0x03, 0x00, 0x05, 0x00, 0x00, 0x06, 0xF5};
//uint8_t ERROR1[]    = {0xF5, 0x03, 0x00, 0x06, 0x12, 0x00, 0x17, 0xF5};
//uint8_t TIME_OUT1[] = {0xF5, 0x03, 0x00, 0x00, 0x08, 0x00, 0x0B, 0xF5};

static uint8_t DEL_ALLUSER[]  = {0xF5, 0x05, 0x00, 0x00, 0x01, 0x00, 0x04, 0xF5};


static uint8_t MATCH[]     = {0xF5, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xF5};
//uint8_t SUCCESS1[]  = {0xF5, 0x0C, 0x00, 0x01, 0x01, 0x00, 0x0C, 0xF5};
//uint8_t SUCCESS2[]  = {0xF5, 0x0C, 0x00, 0x02, 0x01, 0x00, 0x0C, 0xF5};
//uint8_t SUCCESS3[]  = {0xF5, 0x0C, 0x00, 0x03, 0x01, 0x00, 0x0E, 0xF5};
//uint8_t SUCCESS4[]  = {0xF5, 0x0C, 0x00, 0x04, 0x01, 0x00, 0x09, 0xF5};
//uint8_t SUCCESS5[]  = {0xF5, 0x0C, 0x00, 0x05, 0x01, 0x00, 0x08, 0xF5};

//uint8_t ERROR2[]    = {0xF5, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xF5};
//uint8_t TIME_OUT2[] = {0xF5, 0x0C, 0x00, 0x00, 0x08, 0x00, 0x04, 0xF5};


// 串口3-USART3
#define  DEBUG_USARTx                   USART3
#define  DEBUG_USART_CLK                RCC_APB1Periph_USART3
#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           9600

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOB)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT         GPIOB   
#define  DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_10
#define  DEBUG_USART_RX_GPIO_PORT       GPIOB
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_11

#define  DEBUG_USART_IRQ                USART3_IRQn
#define  DEBUG_USART_IRQHandler         USART3_IRQHandler



void USART_Config(void);
void Usart_SendByte( uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord(uint8_t * ch);
uint8_t calculateChecksum(uint8_t *data, size_t length); 

void Get_AllUsers(void);
void match_fingerprint(void);
void REGISTER_USER(void);
void DELECT_ALLUSERS(void);
void DELECT_ONEUSER(uint8_t UserNum);

#endif /* __USART_H */
