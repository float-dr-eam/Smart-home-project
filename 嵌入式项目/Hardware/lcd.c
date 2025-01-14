#include "lcd.h"
//#include "stdlib.h"
#include "lcdfont.h"  	 
#include "delay.h"
/***************** TFT-LCD驱动程序 ******************
                      STM32
**********************BEGIN***********************/
void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = LCD_SCL_GPIO_PIN | LCD_SDA_GPIO_PIN | LCD_RST_GPIO_PIN | LCD_DC_GPIO_PIN | LCD_BLK_GPIO_PIN;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
    GPIO_Init(LCD_SCL_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIOA
    GPIO_SetBits(LCD_SCL_GPIO_PORT, LCD_SCL_GPIO_PIN | LCD_SDA_GPIO_PIN | LCD_RST_GPIO_PIN | LCD_DC_GPIO_PIN | LCD_BLK_GPIO_PIN);
}

/******************************************************************************
      函数功能: 初始化LCD显示器的引脚
      输入参数: 无
      输出参数: 无
******************************************************************************/

void LCD_Writ_Bus(u8 dat) 
{	
	u8 i;

	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}	

}


/******************************************************************************
      函数功能: 向LCD写入颜色数据
      输入参数: 颜色数据 (data) - 要写入的颜色值
      输出参数: 无
******************************************************************************/
void LCD_WriteColor(uint16_t color)
{
    // 发送颜色数据到LCD
    // 实现代码...
}

void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数功能: 向LCD写入颜色数据
      输入参数: 颜色数据 (dat) - 要写入的颜色值
      输出参数: 无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat >> 8); // 先发送高字节
    LCD_Writ_Bus(dat);      // 再发送低字节
}

/******************************************************************************
      函数功能: 向LCD写入命令
      输入参数: 命令数据 (dat) - 要写入的命令值
      输出参数: 无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr(); // 设置为命令模式
    LCD_Writ_Bus(dat); // 发送命令
    LCD_DC_Set(); // 设置为数据模式
}

/******************************************************************************
      函数功能: 设置显示区域
      输入参数: 
          x1, y1 - 左上角坐标
          x2, y2 - 右下角坐标
      输出参数: 无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2a); // 设置列地址
    LCD_WR_DATA(x1);  // 发送左边界
    LCD_WR_DATA(x2);  // 发送右边界

    LCD_WR_REG(0x2b); // 设置页地址
    LCD_WR_DATA(y1);  // 发送上边界
    LCD_WR_DATA(y2);  // 发送下边界

    LCD_WR_REG(0x2c); // 进入写数据模式
}


void LCD_Init(void)
{
	LCD_GPIO_Init();
	
	LCD_RES_Clr();
	Delay_ms(20);
	LCD_RES_Set();
	Delay_ms(20);

	//------------------------------------ST7789 Frame Rate-----------------------------------------// 
	LCD_WR_REG(0x36); 
	LCD_WR_DATA8(0x00);
	
	LCD_WR_REG(0x3A); 
	LCD_WR_DATA8(0x05);
	
	LCD_WR_REG(0xB2); 
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x33); 
	LCD_WR_DATA8(0x33); 
	
	LCD_WR_REG(0xB7); 
	LCD_WR_DATA8(0x35);

	//------------------------------------End ST7789 Frame Rate---------------------------------// 
	LCD_WR_REG(0xBB); //Dot inversion 
	LCD_WR_DATA8(0x19); 
	//------------------------------------ST7789 Power Sequence---------------------------------// 
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA8(0x2C); 

	LCD_WR_REG(0xC2); 
	LCD_WR_DATA8(0X01);
	
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x12);
	

	LCD_WR_REG(0xC4); 
	LCD_WR_DATA8(0x20); 
	
 
	LCD_WR_REG(0xC6); 
	LCD_WR_DATA8(0x0F); 

	//---------------------------------End ST7789 Power Sequence-------------------------------------// 
	LCD_WR_REG(0xD0); //VCOM 
	LCD_WR_DATA8(0xA4); 
	LCD_WR_DATA8(0xA1);

	//------------------------------------ST7789 Gamma Sequence---------------------------------// 
	LCD_WR_REG(0xE0); 
	LCD_WR_DATA8(0xD0); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x11); 
	LCD_WR_DATA8(0x13); 
	LCD_WR_DATA8(0x2B); 
	LCD_WR_DATA8(0x3F); 
	LCD_WR_DATA8(0x54); 
	LCD_WR_DATA8(0x4C); 
	LCD_WR_DATA8(0x18); 
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x0B); 
	LCD_WR_DATA8(0x1F); 
	LCD_WR_DATA8(0x23); 
 
	LCD_WR_REG(0xE1); 
	LCD_WR_DATA8(0xD0); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x0C); 
	LCD_WR_DATA8(0x11); 
	LCD_WR_DATA8(0x13); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x3F); 
	LCD_WR_DATA8(0x44); 
	LCD_WR_DATA8(0x51); 
	LCD_WR_DATA8(0x2F); 
	LCD_WR_DATA8(0x1F); 
	LCD_WR_DATA8(0x1F); 
	LCD_WR_DATA8(0x20); 
	LCD_WR_DATA8(0x23); 
//	LCD_WR_DATA8(0x04); 
//	LCD_WR_DATA8(0x13); 
	//------------------------------------End ST7789 Gamma Sequence-----------------------------// 
	LCD_WR_REG(0x21); //65k mode 
	LCD_WR_REG(0x11); //
	Delay_ms(20);
//	LCD_WR_DATA8(0x05); 
	LCD_WR_REG(0x29); //Display on 
} 



/******************************************************************************
      函数功能: 填充指定区域的颜色
      输入参数: 
          xsta, ysta   - 左上角坐标
          xend, yend   - 右下角坐标
          color        - 需要填充的颜色
      输出参数: 无
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{          
    u16 i, j; 
    LCD_Address_Set(xsta, ysta, xend, yend); // 设置绘制区域
    for(i = ysta; i < yend; i++)
    {                                                      
        for(j = xsta; j < xend; j++)
        {
            LCD_WR_DATA(color); // 写入颜色数据
        }
    }                       
}

/******************************************************************************
      函数功能: 绘制一个点
      输入参数: 
          x, y   - 点的坐标
          color   - 点的颜色
      输出参数: 无
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Address_Set(x, y, x, y); // 设置绘制点的坐标
    LCD_WR_DATA(color); // 写入颜色数据
} 

/******************************************************************************
      函数功能: 绘制一条线
      输入参数: 
          x1, y1   - 起点坐标
          x2, y2   - 终点坐标
          color     - 线的颜色
      输出参数: 无
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // 计算x方向的变化量
    delta_y = y2 - y1;
    uRow = x1; // 当前点的x坐标
    uCol = y1; // 当前点的y坐标
    if(delta_x > 0) incx = 1; // x方向正向移动
    else if (delta_x == 0) incx = 0; // x方向不变
    else { incx = -1; delta_x = -delta_x; }
    
    if(delta_y > 0) incy = 1; // y方向正向移动
    else if (delta_y == 0) incy = 0; // y方向不变
    else { incy = -1; delta_y = -delta_y; }
    
    if(delta_x > delta_y) distance = delta_x; // 选择变化较大的方向
    else distance = delta_y;
    
    for(t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color); // 绘制当前点
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx; // 更新x坐标
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy; // 更新y坐标
        }
    }
}



// 函数：LCD_DrawRectangle
// 描述：在LCD上绘制一个矩形
// 参数：
// x1, y1 - 矩形左上角坐标
// x2, y2 - 矩形右下角坐标
// color - 颜色
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color) {
    LCD_DrawLine(x1, y1, x2, y1, color); // 绘制上边
    LCD_DrawLine(x1, y1, x1, y2, color); // 绘制左边
    LCD_DrawLine(x1, y2, x2, y2, color); // 绘制下边
    LCD_DrawLine(x2, y1, x2, y2, color); // 绘制右边
}

// 函数：Draw_Circle
// 描述：使用中点圆算法绘制一个圆
// 参数：
// x0, y0 - 圆心坐标
// r - 半径
// color - 颜色
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color) {
    int a = 0, b = r; // 初始化参数
    while (a <= b) {
        // 在所有八个象限绘制点
        LCD_DrawPoint(x0 - b, y0 - a, color);
        LCD_DrawPoint(x0 + b, y0 - a, color);
        LCD_DrawPoint(x0 - a, y0 + b, color);
        LCD_DrawPoint(x0 - a, y0 - b, color);
        LCD_DrawPoint(x0 + b, y0 + a, color);
        LCD_DrawPoint(x0 + a, y0 - b, color);
        LCD_DrawPoint(x0 + a, y0 + b, color);
        LCD_DrawPoint(x0 - b, y0 + a, color);
        
        a++; // 增加 a 的值
        if ((a * a + b * b) > (r * r)) { // 检测是否超出半径
            b--; // 减少 b 的值
        }
    }
}

// 函数：LCD_ShowChinese
// 描述：在LCD上显示中文字符
// 参数：
// x, y - 显示位置
// s - 字符串指针
// fc - 前景色
// bc - 背景色
// sizey - 字体大小
// mode - 显示模式
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    while (*s != 0) { // 遍历字符串
        if (sizey == 12) 
            LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 16) 
            LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 24) 
            LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 32) 
            LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
        else 
            return; // 如果字体大小不支持则退出
        s += 2; // 移动到下一个字符
        x += sizey; // 更新下一个字符的x坐标
    }
}

// 函数：LCD_ShowChinese12x12
// 描述：绘制12x12的中文字符
// 参数：
// x, y - 显示位置
// s - 字符串指针
// fc - 前景色
// bc - 背景色
// sizey - 字体大小
// mode - 显示模式
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    u8 t, i, j,longsize; // 循环变量
    u16 k;
    u16 HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // 字符数量
    u16 x0 = x, y0 = y; // 保存起始位置
    u16 TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey; // 字体高度
    longsize = (sizeof(s))/NB;
    // 遍历每个字符
    for (t = 0; t < longsize; t++) {
        for (k = 0; k < HZnum; k++) {
            if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1))&&(tfont12[k].Index[2]==*(s+2))) 
			{
                s+=NB; // 移动到下一个字符
                // 渲染字符
                for (i = 0; i < TypefaceNum; i++) {
                    for (j = 0; j < 8; j++) {
                        if (!mode) { // 正常模式
                            if (tfont12[k].Msk[i] & (0x01 << j))
                                LCD_DrawPoint(x, y, fc); // 绘制前景色
                            else
                                LCD_DrawPoint(x, y, bc); // 绘制背景色
                            x++;
                            // 检查是否换行
                            if ((x - x0) == sizey) {
                                x = x0;
                                y++;
                                if ((y - y0) == sizey) {
                                    x += sizey;
                                    x0 = x;
                                    y = y0;
                                }
                                break;
                            }
                        } else { // 反向模式
                            if (tfont12[k].Msk[i] & (0x01 << j))
                                LCD_DrawPoint(x, y, fc); // 绘制前景色
                            x++;
                            if ((x - x0) == sizey) {
                                x = x0;
                                y++;
                                if ((y - y0) == sizey) {
                                    x += sizey;
                                    x0 = x;
                                    y = y0;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}



void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,longsize,t=0;
	u16 k;
	u16 HZnum;
	u16 TypefaceNum;
	u16 x0=x;
	u16 y0=y;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	
	longsize = (sizeof(s))/NB;
	for(t=0;t<longsize;t++)
	{
		for(k=0;k<HZnum;k++) 
		{
			if((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1))&&(tfont16[k].Index[2]==*(s+2)))
			{ 	
				s+=NB;
				//LCD_Address_Set(x+t*(sizey-1),y,x+(t+1)*(sizey-1),y+sizey-1);
				for(i=0;i<TypefaceNum;i++)
				{
					for(j=0;j<8;j++)
					{	
						if(!mode)
						{
                            if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
							else 	LCD_DrawPoint(x,y,bc);
							x++;
							if((x-x0)==sizey)
							{
								x=x0;
								y++;
								if((y-y0)==sizey)
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
						else
						{
							if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
							x++;
							if((x-x0)==sizey)
							{
								x=x0;
								y++;
								if((y-y0)==sizey)
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
					}
				}
			}				  	
			continue;  
		}
	}
} 



void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,longsize,t=0;
	u16 k;
	u16 HZnum;
	u16 TypefaceNum;
	u16 x0=x;
	u16 y0=y;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//每个字符的字模长度/8
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);//结构体数组的长度	
	longsize = (sizeof(s))/NB;//字符串长度
	for(t=0;t<longsize;t++)
	{
		for(k=0;k<HZnum;k++) 
		{
			if((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1))&&(tfont24[k].Index[2]==*(s+2)))//找到对应汉字
			{ 	
				s+=NB;
				for(i=0;i<TypefaceNum;i++)//字模长度/8
				{
					for(j=0;j<8;j++)//显示8位一个字节
					{	
						if(!mode)
						{
							if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//前景色
							else LCD_DrawPoint(x,y,bc);//背景色
							x++;
							if((x-x0)==sizey)//指针递增
							{
								x=x0;
								y++;
								if((y-y0)==sizey)//指针递增
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
						else
						{
							if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
							x++;
							if((x-x0)==sizey)
							{
								x=x0;
								y++;
								if((y-y0)==sizey)
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
					}
				}
				
			}				  	
			continue;  
		}
	}
} 


void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,longsize,t=0;
	u16 k;
	u16 HZnum;
	u16 TypefaceNum;
	u16 x0=x;
	u16 y0=y;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	
	longsize = (sizeof(s))/NB;
	for(t=0;t<longsize;t++)
	{
		for(k=0;k<HZnum;k++) 
		{
			if((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1))&&(tfont32[k].Index[2]==*(s+2)))
			{ 	
				s+=NB;
				for(i=0;i<TypefaceNum;i++)
				{
					for(j=0;j<8;j++)
					{	
						if(!mode)
						{
                            if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
							else LCD_DrawPoint(x,y,bc);
							x++;
							if((x-x0)==sizey)
							{
								x=x0;
								y++;
								if((y-y0)==sizey)
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
						else
						{
							if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
							x++;
							if((x-x0)==sizey)
							{
								x=x0;
								y++;
								if((y-y0)==sizey)
								{
									x+=sizey;
									x0=x;
									y=y0;
								}	
								break;
							}
						}
					}
				}
			}				  	
			continue;  
		}
	}
}


/******************************************************************************
      函数名称: LCD_ShowChar
      功能描述: 在指定坐标(x,y)显示一个字符
      输入参数: 
                x       字符显示的起始横坐标
                y       字符显示的起始纵坐标
                num     要显示的字符（ASCII码）
                fc      字符的前景色
                bc      字符的背景色
                sizey   字符的高度
                mode    0 - 垂直模式，1 - 水平模式
      返回值:   无
******************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 temp, sizex, t, m = 0;
	u16 i, TypefaceNum; // 字体大小
	u16 x0 = x;
	sizex = sizey / 2; // 计算字符宽度
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey; // 计算字形所需的字节数
	num = num - ' '; // 转换为对应的字符索引
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // 设置显示区域

	for(i = 0; i < TypefaceNum; i++)
	{ 
		if(sizey == 12) temp = ascii_1206[num][i]; // 选择6x12字体
		else if(sizey == 16) temp = ascii_1608[num][i]; // 选择8x16字体
		else if(sizey == 24) temp = ascii_2412[num][i]; // 选择12x24字体
		else if(sizey == 32) temp = ascii_3216[num][i]; // 选择16x32字体
		else return; // 如果sizey不在支持范围内，则返回

		for(t = 0; t < 8; t++)
		{
			if(!mode) // 垂直模式
			{
				if(temp & (0x01 << t)) LCD_WR_DATA(fc); // 前景色
				else LCD_WR_DATA(bc); // 背景色
				m++;
				if(m % sizex == 0) // 一行结束，换行
				{
					m = 0;
					break;
				}
			}
			else // 水平模式
			{
				if(temp & (0x01 << t)) LCD_DrawPoint(x, y, fc); // 绘制点
				x++;
				if((x - x0) == sizex) // 一行结束，换行
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}

/******************************************************************************
      函数名称: LCD_ShowString
      功能描述: 在指定坐标(x,y)显示字符串
      输入参数: 
                x       字符串显示的起始横坐标
                y       字符串显示的起始纵坐标
                *p      指向要显示的字符串的指针
                fc      字符的前景色
                bc      字符的背景色
                sizey   字符的高度
                mode    0 - 垂直模式，1 - 水平模式
      返回值:   无
******************************************************************************/
void LCD_ShowString(u16 x, u16 y, u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{         
	while(*p != '\0') // 当字符串未结束
	{       
		LCD_ShowChar(x, y, *p, fc, bc, sizey, mode); // 显示当前字符
		x += sizey / 2; // 更新横坐标
		p++; // 指向下一个字符
	}  
}

/******************************************************************************
      函数名称: mypow
      功能描述: 计算m的n次方
      输入参数: 
                m       底数
                n       指数
      返回值:   结果
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;	 
	while(n--) result *= m; // 循环计算乘法
	return result; // 返回结果
}


/******************************************************************************
      函数名称: LCD_ShowIntNum
      功能描述: 在指定坐标(x, y)显示一个整数
      输入参数: 
                x       显示的起始横坐标
                y       显示的起始纵坐标
                num     需要显示的整数
                len     显示的位数
                fc      字体颜色
                bc      背景颜色
                sizey   字体大小
      返回值:   无
******************************************************************************/
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{         
    u8 t, temp; // 循环变量和临时存储
    u8 enshow = 0; // 是否显示标志
    u8 sizex = sizey / 2; // 计算每个字符的宽度
    for(t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10; // 获取当前位的数字
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0) // 如果当前位是0且不是最后一位
            {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0); // 显示空格
                continue;
            }
            else enshow = 1; // 开始正式显示
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0); // 显示当前位数字
    }
}

/******************************************************************************
      函数名称: LCD_ShowFloatNum1
      功能描述: 在指定坐标(x, y)显示一个浮点数
      输入参数: 
                x       显示的起始横坐标
                y       显示的起始纵坐标
                num     需要显示的浮点数
                len     显示的总位数（包括小数点）
                fc      字体颜色
                bc      背景颜色
                sizey   字体大小
      返回值:   无
******************************************************************************/
void LCD_ShowFloatNum(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{         
    u8 t, temp, sizex; // 循环变量和临时存储
    u16 num1; // 用于存储整数部分
    sizex = sizey / 2; // 计算每个字符的宽度
    num1 = num * 100; // 按照小数点后两位的方式处理浮点数
    for(t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10; // 获取当前位的数字
        if(t == (len - 2)) // 如果到达小数点前一位
        {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0); // 显示小数点
            t++;
            len += 1; // 更新长度以显示小数部分
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0); // 显示当前位数字
    }
}


/******************************************************************************
      函数名称: LCD_ShowPicture
      功能描述: 在指定坐标(x,y)显示一幅图像
      输入参数: 
                x       图像显示的起始横坐标
                y       图像显示的起始纵坐标
                length  图像的宽度
                width   图像的高度
                pic[]   指向图像数据的数组
      返回值:   无
******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
    u16 i, j; // 循环变量
    u32 k = 0; // 数据索引
    LCD_Address_Set(x, y, x + length - 1, y + width - 1); // 设置显示区域

    for (i = 0; i < length; i++) // 遍历图像的每一行
    {
        for (j = 0; j < width; j++) // 遍历图像的每一列
        {
            LCD_WR_DATA8(pic[k * 2]);     // 写入当前像素的高字节
            LCD_WR_DATA8(pic[k * 2 + 1]); // 写入当前像素的低字节
            k++; // 更新数据索引
        }
    }            
}

void showimage(const u8 image[]) 
{
  	int i,j,k; 

	for(k=0;k<6;k++)
	{
	   	for(j=0;j<6;j++)
		{	
			LCD_Address_Set(40*j,40*k,40*j+39,40*k+39);		
		    for(i=0;i<1600;i++)
			 { 	
				 		
			  	 LCD_WR_DATA8(image[i*2+1]);	 
				 LCD_WR_DATA8(image[i*2]);				
			 }	
		 }
	}			
}


