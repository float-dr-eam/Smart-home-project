#ifndef __LCD_H
#define __LCD_H 
#include "sys.h"
	

#define LCD_SCL_GPIO_PORT				GPIOA
#define LCD_SCL_GPIO_PIN				GPIO_Pin_1
				
#define LCD_SDA_GPIO_PORT				GPIOA
#define LCD_SDA_GPIO_PIN				GPIO_Pin_4
				
#define LCD_RST_GPIO_PORT				GPIOA
#define LCD_RST_GPIO_PIN				GPIO_Pin_5
				
#define LCD_DC_GPIO_PORT				GPIOA
#define LCD_DC_GPIO_PIN					GPIO_Pin_6
				
//#define LCD_CS_GPIO_PORT				GPIOA
//#define LCD_CS_GPIO_PIN					GPIO_Pin_4

#define LCD_BLK_GPIO_PORT				GPIOA
#define LCD_BLK_GPIO_PIN				GPIO_Pin_7

/*********************END**********************/


#define LCD_SCLK_Clr() GPIO_ResetBits(LCD_SCL_GPIO_PORT,LCD_SCL_GPIO_PIN)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(LCD_SCL_GPIO_PORT,LCD_SCL_GPIO_PIN)

#define LCD_MOSI_Clr() GPIO_ResetBits(LCD_SDA_GPIO_PORT,LCD_SDA_GPIO_PIN)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(LCD_SDA_GPIO_PORT,LCD_SDA_GPIO_PIN)

#define LCD_RES_Clr()  GPIO_ResetBits(LCD_RST_GPIO_PORT,LCD_RST_GPIO_PIN)//RES
#define LCD_RES_Set()  GPIO_SetBits(LCD_RST_GPIO_PORT,LCD_RST_GPIO_PIN)

#define LCD_DC_Clr()   GPIO_ResetBits(LCD_DC_GPIO_PORT,LCD_DC_GPIO_PIN)//DC
#define LCD_DC_Set()   GPIO_SetBits(LCD_DC_GPIO_PORT,LCD_DC_GPIO_PIN)
 		     
//#define LCD_CS_Clr()   GPIO_ResetBits(LCD_CS_GPIO_PORT,LCD_CS_GPIO_PIN)//CS
//#define LCD_CS_Set()   GPIO_SetBits(LCD_CS_GPIO_PORT,LCD_CS_GPIO_PIN)

#define LCD_BLK_Clr()  GPIO_ResetBits(LCD_BLK_GPIO_PORT,LCD_BLK_GPIO_PIN)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(LCD_BLK_GPIO_PORT,LCD_BLK_GPIO_PIN)


#define LCD_W 240
#define LCD_H 240
#define NB 3

#define WHITE           0xFFFF // 白色
#define BLACK           0x0000 // 黑色
#define BLUE            0x001F // 蓝色
#define BRED            0xF81F // 亮红色
#define GRED            0xFFE0 // 亮绿色
#define GBLUE           0x07FF // 亮蓝色
#define RED             0xF800 // 红色
#define MAGENTA         0xF81F // 品红色
#define GREEN           0x07E0 // 绿色
#define CYAN            0x7FFF // 青色
#define YELLOW          0xFFE0 // 黄色
#define BROWN           0xBC40 // 棕色
#define BRRED           0xFC07 // 深红色
#define GRAY            0x8430 // 灰色
#define DARKBLUE        0x01CF // 深蓝色
#define LIGHTBLUE       0x7D7C // 浅蓝色  
#define GRAYBLUE        0x5458 // 灰蓝色
#define LIGHTGREEN      0x841F // 浅绿色
#define LGRAY           0xC618 // 浅灰色
#define LGRAYBLUE       0xA651 // 浅灰蓝色
#define LBBLUE          0x2B12 // 浅蓝色





void LCD_GPIO_Init(void); // 初始化GPIO
void LCD_Writ_Bus(u8 dat); // 通过SPI写入数据
void LCD_WR_DATA8(u8 dat); // 写入8位数据
void LCD_WR_DATA(u16 dat); // 写入16位数据
void LCD_WR_REG(u8 dat); // 写入寄存器
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2); // 设置显示区域

void LCD_Init(void); // LCD初始化

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color); // 填充矩形区域指定颜色
void LCD_DrawPoint(u16 x, u16 y, u16 color); // 绘制一个点
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color); // 绘制一条线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color); // 绘制矩形
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color); // 绘制圆形

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示汉字
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示12x12汉字
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示16x16汉字
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示24x24汉字
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示32x32汉字

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示一个字符
void LCD_ShowString(u16 x, u16 y, u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示字符串
u32 mypow(u8 m, u8 n); // 幂运算
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey); // 显示整数
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey); // 显示浮点数

void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]); // 显示图片

void showimage(const u8 image[]); // 显示图像

#endif

