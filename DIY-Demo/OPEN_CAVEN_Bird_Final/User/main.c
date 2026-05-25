#include "center_app.h"
#include "Mode.h"

#include "lvgl.h"
#include "ui.h"

#define BUFF_LEN 5

static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf_1[MY_DISP_HOR_RES * BUFF_LEN];
static lv_disp_drv_t disp_drv;
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);
	// M5.Lcd.drawBitmap(area->x1, area->y1, w, h, &color_p->full);
	// lcd_show_image(area->x1, area->y1, w, h, &color_p->full);
    Mode_Use.LCD.Show_Picture_pFun(area->x1, area->y1, w, h, (u8*)&color_p->full); // Photo
	lv_disp_flush_ready(disp_drv);
}

uint32_t custom_millis(void)
{
	uint32_t retval;
    Caven_BaseTIME_Type now_time;
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
	retval = now_time.SYS_Us / 1000;
	retval += now_time.SYS_Sec * 1000;
	return retval;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */
static void hal_init(void)
{

}
void gui_init (void)
{
	/*Initialize LVGL*/
	lv_init();
	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();
	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * BUFF_LEN);
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = MY_DISP_HOR_RES;
	disp_drv.ver_res = MY_DISP_VER_RES;
	disp_drv.flush_cb = disp_flush;
	disp_drv.draw_buf = &draw_buf_dsc_1;
	lv_disp_drv_register(&disp_drv);
	ui_init();
}

void Main_Init(void);

int main(void)
{
    Caven_BaseTIME_Type now_time;
	
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
	
	while(1)
    {
		now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

		if(Center_State_machine(now_time))          // 状态机入口
		{
			break;									// 状态机退出,程序重启
		}
    }
	Mode_Use.TIME.Delay_Ms (250);
    SYS_RESET();
}

void Main_Init(void)
{
    Mode_Index();
	
	Center_app_Init ();
	System_app_Init ();
}


