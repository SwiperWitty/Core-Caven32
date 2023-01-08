#include "LCD.h"
#include "lcdfont.h" //字库

U16 BACK_COLOR = BLACK; //背景色


#ifdef Exist_LCD
void LCD_Writ_Bus(U8 dat)
{
	SPI_Send_DATA(dat);
}

void LCD_Writ_String(const void * DATA,int num)
{
    SPI_Send_String(DATA,num);
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_WR_DATA8(U8 dat)
{
	SPI_CS_Set(1,ENABLE);
	LCD_Writ_Bus(dat);
	SPI_CS_Set(1,DISABLE);
}

void LCD_WR_DATA(U16 dat)
{
	SPI_CS_Set(1,ENABLE);
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat & 0X00FF);
	SPI_CS_Set(1,DISABLE);
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_Send_Data(U8 *Data,int num)
{
	SPI_CS_Set(1,ENABLE);
	for (size_t i = 0; i < num; i++)
	{
		LCD_Writ_Bus(*(Data + i));
	}
	SPI_CS_Set(1,DISABLE);
}

/******************************************************************************
	  函数说明：LCD写入命令
	  入口数据：dat 写入的命令
	  返回值：  无
******************************************************************************/
void LCD_WR_CMD(U8 dat)
{
    SPI_CS_Set(1,ENABLE);
	LCD_DC_L(); //写命令
	LCD_Writ_Bus(dat);
	LCD_DC_H(); //写数据	预备
    SPI_CS_Set(1,DISABLE);
}

/******************************************************************************
	  函数说明：设置起始和结束地址
	  入口数据：x1,x2 设置列的起始和结束地址
				y1,y2 设置行的起始和结束地址
	  返回值：  无
******************************************************************************/
void LCD_Address_Set(U16 x1, U16 y1, U16 x2, U16 y2)
{
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_CMD(0x2a); //列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_CMD(0x2b); //行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_CMD(0x2c); //储存器写
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_CMD(0x2a); //列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_CMD(0x2b); //行地址设置
		LCD_WR_DATA(y1 + 80);
		LCD_WR_DATA(y2 + 80);
		LCD_WR_CMD(0x2c); //储存器写
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_CMD(0x2a); //列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_CMD(0x2b); //行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_CMD(0x2c); //储存器写
	}
	else
	{
		LCD_WR_CMD(0x2a); //列地址设置
		LCD_WR_DATA(x1 + 80);
		LCD_WR_DATA(x2 + 80);
		LCD_WR_CMD(0x2b); //行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_CMD(0x2c); //储存器写
	}
}
#endif
//以上不提供到其他文件

/******************************************************************************
	  函数说明：在指定区域填充颜色
	  入口数据：xsta,ysta   起始坐标
				xend,yend   终止坐标
								color       要填充的颜色
	  返回值：  无
******************************************************************************/
void LCD_Fill(U16 x_sta, U16 y_sta, U16 x_end, U16 y_end, U16 color)
{
#ifdef Exist_LCD
    int i = (x_end - x_sta) * (y_end - y_sta);
	LCD_Address_Set(x_sta, y_sta, x_end - 1, y_end - 1); //设置显示范围
    for(;i > 0;i--)
    {
        LCD_WR_DATA(color);
    }

#endif
}

/******************************************************************************
	  函数说明：在指定位置画点
	  入口数据：x,y 画点坐标
				color 点的颜色
	  返回值：  无
******************************************************************************/
void LCD_Draw_Point(U16 x, U16 y, U16 color)
{
#ifdef Exist_LCD
	LCD_Address_Set(x, y, x, y); //设置光标位置
	LCD_WR_DATA(color);
#endif
}

/******************************************************************************
	  函数说明：画线
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
				color   线的颜色
	  返回值：  无
******************************************************************************/
void LCD_Draw_Line(U16 x1, U16 y1, U16 x2, U16 y2, U16 color)
{
#ifdef Exist_LCD
	U16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; //画线起点坐标
	uCol = y1;
	if (delta_x > 0)
		incx = 1; //设置单步方向
	else if (delta_x == 0)
		incx = 0; //垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; //水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; //选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_Draw_Point(uRow, uCol, color); //画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
#endif
}

/******************************************************************************
	  函数说明：画矩形
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
				color   矩形的颜色
	  返回值：  无
******************************************************************************/
void LCD_Draw_Rectangle(U16 x1, U16 y1, U16 x2, U16 y2, U16 color)
{
	LCD_Draw_Line(x1, y1, x2, y1, color);
	LCD_Draw_Line(x1, y1, x1, y2, color);
	LCD_Draw_Line(x1, y2, x2, y2, color);
	LCD_Draw_Line(x2, y1, x2, y2, color);
}

/******************************************************************************
	  函数说明：画圆
	  入口数据：x0,y0   圆心坐标
				r       半径
				color   圆的颜色
	  返回值：  无
******************************************************************************/
void LCD_Draw_Circle(U16 x0, U16 y0, char r, U16 color)
{
	int a, b;
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_Draw_Point(x0 - b, y0 - a, color); // 3
		LCD_Draw_Point(x0 + b, y0 - a, color); // 0
		LCD_Draw_Point(x0 - a, y0 + b, color); // 1
		LCD_Draw_Point(x0 - a, y0 - b, color); // 2
		LCD_Draw_Point(x0 + b, y0 + a, color); // 4
		LCD_Draw_Point(x0 + a, y0 - b, color); // 5
		LCD_Draw_Point(x0 + a, y0 + b, color); // 6
		LCD_Draw_Point(x0 - b, y0 + a, color); // 7
		a++;
		if ((a * a + b * b) > (r * r)) //判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
	  函数说明：显示单个24x24汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_Show_Chinese24x24(U16 x, U16 y, char *s, U16 fc, U16 bc, char sizey, char mode)
{
#ifdef String_Chinese
	char i, j, m = 0;
	U16 k;
	U16 HZnum;		 //汉字数目
	U16 TypefaceNum; //一个字符所占字节大小
	U16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); //统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) //非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else //叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_Draw_Point(x, y, fc); //画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
#endif
}

/******************************************************************************
	  函数说明：显示单个32x32汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_Show_Chinese32x32(U16 x, U16 y, char *s, U16 fc, U16 bc, char sizey, char mode)
{
#ifdef String_Chinese
	char i, j, m = 0;
	U16 k;
	U16 HZnum;		 //汉字数目
	U16 TypefaceNum; //一个字符所占字节大小
	U16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); //统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) //非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else //叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_Draw_Point(x, y, fc); //画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
#endif
}

/******************************************************************************
	  函数说明：显示单个字符
	  入口数据：x,y显示坐标
				num 要显示的字符
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_Show_Char(U16 x, U16 y, char num, U16 fc, U16 bc, char sizey, char mode)
{
#ifdef String_Lib
	char temp, sizex, t, m = 0;
	U16 i, TypefaceNum; //一个字符所占字节大小
	U16 x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' ';									 //得到偏移后的值
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); //设置光标位置
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 16)
		{temp = ascii_1608[num][i];} //调用8x16字体
		else if (sizey == 24)
		{temp = ascii_2412[num][i];} //调用12x24字体
		else if (sizey == 32)
		{temp = ascii_3216[num][i];} //调用16x32字体
		else
		{return;}
		for (t = 0; t < 8; t++)
		{
			if (!mode) //非叠加模式
			{
				if (temp & (0x01 << t))
				{LCD_WR_DATA(fc);}
				else
				{LCD_WR_DATA(bc);}
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else //叠加模式
			{
				if (temp & (0x01 << t))
				{LCD_Draw_Point(x, y, fc);} //画一个点
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
#endif
}
/*    以上为文字最小单元      */

/******************************************************************************
	  函数说明：显示汉字串
	  入口数据：x,y显示坐标
				*s 要显示的汉字串
				fc 字的颜色
				bc 字的背景色
				sizey 字号 可选 24 32
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_Show_Chinese(U16 x, U16 y, char *s, U16 fc, U16 bc, char sizey, char mode)
{
	while (*s != 0)
	{
		if (sizey == 24)
		{LCD_Show_Chinese24x24(x, y, s, fc, bc, sizey, mode);}
		else if (sizey == 32)
		{LCD_Show_Chinese32x32(x, y, s, fc, bc, sizey, mode);}
		else
		{return;}
		s += 2;
		x += sizey;
	}
}

/******************************************************************************
	  函数说明：显示字符串
	  入口数据：x,y显示坐标
				*p 要显示的字符串
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_Show_String(U16 x, U16 y, const char *p, U16 fc, U16 bc, char sizey)
{
	if (sizey == 16 || sizey == 24 || sizey == 32)
	{
		x *= (sizey / 2);
		y *= (sizey);
		if (x > LCD_W)
		{x = LCD_W;}
		if (y > LCD_H)
		{y = LCD_W;}
	}
	else
		return;

	while (*p != '\0')
	{
		LCD_Show_Char(x, y, *p, fc, bc, sizey, 0);
		x += sizey / 2;
		p++;
	}
}

/******************************************************************************
	  函数说明：显示图片
	  入口数据：x,y起点坐标
				length 图片长度
				width  图片宽度
				pic[]  图片数组
	  返回值：  无
******************************************************************************/
void LCD_Show_Picture(U16 x,U16 y,U16 length,U16 width,const U8 pic[])
{
#ifdef Exist_LCD
	u16 i,j;
	u32 k = 0;
	LCD_Address_Set(x,y,(x+length-1),(y+width-1));
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);
			LCD_WR_DATA8(pic[k*2+1]);
			k++;
		}
		
	}
#endif
}

#ifdef Exist_LCD
static void LCD_Delay(int time)
{
	int temp;
	for (int i = 0; i < time; ++i)
	{
		temp = 10000; // SET
		while ((--temp) > 0)
			;
	}
}
#endif

void LCD_Init(int Set)
{
#ifdef Exist_LCD
	LCD_GPIO_Init(Set);
	SPI_Start_Init(Set);
	LCD_Delay(200); //等待电路复位完成

//	LCD_RES_Clr();LCD_Delay (20);				//Caven3.14 使用硬件复位，不需要这个
//	LCD_RES_Set();LCD_Delay (20);
	LCD_WR_CMD(0x11); //Sleep out
	LCD_Delay (20);
	LCD_WR_CMD(0x36);
	LCD_Delay(50);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x00);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC0);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x70);
	else
		LCD_WR_DATA8(0xA0);

	LCD_WR_CMD(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_CMD(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_CMD(0xB7);
	LCD_WR_DATA8(0x35);

	LCD_WR_CMD(0xBB);
	LCD_WR_DATA8(0x19);

	LCD_WR_CMD(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_CMD(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_CMD(0xC3);
	LCD_WR_DATA8(0x12);

	LCD_WR_CMD(0xC4);
	LCD_WR_DATA8(0x20);

	LCD_WR_CMD(0xC6);
	LCD_WR_DATA8(0x0F);

	LCD_WR_CMD(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_CMD(0xE0);
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

	LCD_WR_CMD(0xE1);
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

	LCD_WR_CMD(0x21);
	LCD_WR_CMD(0x11);
	LCD_WR_CMD(0x29);
	LCD_Delay(200);
	LCD_Fill(0, 0, LCD_W, LCD_H, BACK_COLOR);
#endif
}
