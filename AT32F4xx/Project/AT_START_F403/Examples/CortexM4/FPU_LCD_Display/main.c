/**
  ******************************************************************************
  * File   : CortexM4/FPU_LCD_Display/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
 
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "timer.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 

/* FPU Pattern hints-------------------------------------*/
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On" 
#else
#define SCORE_FPU_MODE                  "FPU Off" 
#endif

#define 	ITERATION			128			//迭代次数
#define	 	REAL_CONSTANT		0.285f		//实部常量
#define	 	IMG_CONSTANT		0.01f		//虚部常量

/* Color table-------------------------------------*/
u16 color_map[ITERATION];

/* Scale factor list -------------------------------------*/
const u16 zoom_ratio[] =
{
	120, 110, 100, 150, 200, 275, 350, 450,
	600, 800, 1000, 1200, 1500, 2000, 1500,
	1200, 1000, 800, 600, 450, 350, 275, 200,
	150, 100, 110,
}; 


/**
  * @brief  Initializes the color list.
  * @param  None
  * @retval clut: Pointer of color list.
  */
void InitCLUT(u16 * clut)
{
 	u32 i=0x00;
	u16  red=0,green=0,blue=0;  
	for(i=0;i<ITERATION;i++)//产生颜色表
	{
		//产生RGB颜色值
		red=(i*8*256/ITERATION)%256;
		green=(i*6*256/ITERATION)%256;
		blue=(i*4*256 /ITERATION)%256; 
 		//将RGB888,转换为RGB565
		red=red>>3;
		red=red<<11;
		green=green>>2;
		green=green<<5;
		blue=blue>>3; 
		clut[i]=red+green+blue;
	}
} 

/**
  * @brief  Create the Julia Fractal image.
  * @param  size_x,size_y:the size of the screen x,y direction.
  *         offset_x,offset_y:screen x,y direction shift.
  *z        oom: Scaling factor.
  * @retval None
  */
void GenerateJulia_fpu(u16 size_x,u16 size_y,u16 offset_x,u16 offset_y,u16 zoom)
{
	u8 i;
	u16 x,y;  
	float tmp1,tmp2;
	float num_real,num_img;
	float radius; 
	for(y=0;y<size_y;y++)
	{
       	for(x=0;x<size_x;x++)
   		{
       		num_real=y-offset_y;
         	num_real=num_real/zoom;
           	num_img=x-offset_x;
          	num_img=num_img/zoom;
           	i=0;
          	radius=0;
           	while((i<ITERATION-1)&&(radius<4))
           	{
           		tmp1=num_real*num_real;
               	tmp2=num_img*num_img;
              	num_img=2*num_real*num_img+IMG_CONSTANT;
             	num_real=tmp1-tmp2+REAL_CONSTANT;
               	radius=tmp1+tmp2;
              	i++;
           	}
			LCD->LCD_RAM=color_map[i];//绘制到屏幕 
       	}
	}
}
u8 timeout;

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{ 
	u8 key;
	u8 i=0;
	u8 autorun=0;
	float time; 
	u8 buf[50];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(); //初始化延时函数
	Uart1_Init(115200);		//初始化串口波特率为115200
	
	LED_Init();					//初始化LED
	KEY_Init();
 	LCD_Init();
 
	TMR3_Int_Init(65535,8400-1);//10Khz计数频率,最大计时6.5秒超出
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer AT32F4");	
	LCD_ShowString(30,70,200,16,16,"FPU TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/7/2");	
	LCD_ShowString(30,130,200,16,16,"KEY0:+    KEY2:-");	//显示提示信息
	LCD_ShowString(30,150,200,16,16,"KEY_UP:AUTO/MANUL");	//显示提示信息	
	delay_ms(500);
  delay_ms(500);
  delay_ms(500);
	POINT_COLOR=BLUE;	//设置字体为蓝色 
	InitCLUT(color_map);//初始化颜色表
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:
				i++;
				if(i>sizeof(zoom_ratio)/2-1)i=0;//限制范围
				break;
			case KEY2_PRES:
				if(i)i--;
				else i=sizeof(zoom_ratio)/2-1;
				break;
			case WKUP_PRES:
				autorun=!autorun;	//自动/手动
				break;
		} 
		if(autorun==1)//自动时,自动设置缩放因子
		{
			i++;
			if(i>sizeof(zoom_ratio)/2-1)i=0;//限制范围
		}
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//设置窗口
		LCD_WriteRAM_Prepare();
		TMR3->CNT=0;//重设 TMR3 定时器的计数器值
		timeout=0;
		GenerateJulia_fpu(lcddev.width,lcddev.height,lcddev.width/2,lcddev.height/2,zoom_ratio[i]);
		time=TMR3->CNT+(u32)timeout*65536;
		sprintf((char*)buf,"%s: zoom:%d  runtime:%0.1fms\r\n",SCORE_FPU_MODE,zoom_ratio[i],time/10);
		LCD_ShowString(5,lcddev.height-5-12,lcddev.width-5,12,12,buf);//显示当前运行情况	 
		printf("%s",buf);//输出到串口
		LED2=!LED2;
	}
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
