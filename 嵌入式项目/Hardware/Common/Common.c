#include "common.h"
#include "stm32f10x.h"
#include <stdarg.h>



static char *   itoa( int value, char * string, int radix );

/*
 * 函数名称：USART2_printf
 * 函数功能：封装输出函数，类似于C标准库的printf，用于通过USART发送数据
 * 输入参数：-USARTx  串口编号，通常为USART2
 *            -Data    待发送的数据内容
 *            -...     其他可选参数
 * 输出结果：无
 * 示例用法：可以这样调用USART2_printf(USART2, "\r\n this is a demo \r\n");
 *             USART2_printf(USART2, "\r\n %d \r\n", i);
 *             USART2_printf(USART2, "\r\n %s \r\n", j);
 */

void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							         
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							         
				USART_SendData(USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}			 
		}
		
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										 
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':		//十进制	
					
				d = va_arg(ap, int);
				
				itoa(d, buf, 10);
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;
				
				default:
				Data++;
				
				break;
				
			}		 
		}
		
		else USART_SendData(USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}


/*

 * 函数名称：itoa
 * 函数功能：将整数转换为字符串
 * 输入参数：-radix = 10 表示进制，通常为10（十进制），可以设置为0
 *            -value 需要转换的整数值
 *            -buf   存储转换后字符串的缓冲区
 *            -radix = 10
 * 输出结果：无
 * 示例用法：可以通过USART2_printf()来调用
 */

static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */



