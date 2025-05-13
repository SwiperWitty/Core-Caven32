/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"


/*
 *
 */

Caven_BaseTIME_Type center_time;
Caven_BaseTIME_Type last_time = {0};
struct tm center_date;

int Center_State_machine(Caven_BaseTIME_Type time)
{
    int retval = 0;
    int temp_num = 0;

    center_time = time;
    center_time = Mode_Use.TIME.Get_BaseTIME_pFun();
    center_date = Mode_Use.TIME.Get_Date_pFun(8*60*60);

    if ((center_time.SYS_Sec != last_time.SYS_Sec)) {
        last_time = center_time;
        printf("date %d/%d/%d %02d:%02d:%02d  utc [%d] [%d]\n",
                center_date.tm_year,center_date.tm_mon,center_date.tm_mday,
                center_date.tm_hour,center_date.tm_min,center_date.tm_sec,
                center_time.SYS_Sec,center_time.SYS_Us);
//        Mode_Use.OLED.Show_String_pFun(0,0,"oled show",0,0,16);
//        Mode_Use.OLED.Refresh();
    }

    return retval;
}

void delay_ms(int milli_seconds)
{
    SYS_Delay_ms(milli_seconds);
}

#if GUI_LVGL
// Transfer GuiLite 32 bits color to your LCD color
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
// Encapsulate your LCD driver:
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
    #ifdef Exist_OLED
    
    #endif
    #ifdef Exist_LCD
    // LCD_Fast_DrawPoint(x, y, GL_RGB_32_to_16(rgb));
    Mode_Use.LCD.Draw_Point_pFun(x, y, GL_RGB_32_to_16(rgb));
    #endif
}
// Implement it, if you have more fast solution than drawing pixels one by one.
// void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb){}

// UI entry
struct DISPLAY_DRIVER
{
    void (*draw_pixel)(int x, int y, unsigned int rgb);
    void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
}my_driver;
extern void startHelloStar(void* phy_fb, int width, int height, int color_bytes, struct DISPLAY_DRIVER* driver);

int start_ui(void)
{
    //Link your LCD driver & start UI:
    my_driver.draw_pixel = gfx_draw_pixel;
    my_driver.fill_rect = NULL;//gfx_fill_rect;
    startHelloStar(NULL, 240, 240, 2, &my_driver);
    return 0;
}

#endif
