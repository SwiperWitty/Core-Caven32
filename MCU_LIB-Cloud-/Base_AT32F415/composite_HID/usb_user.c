#include "cdc_keyboard_class.h"
#include "cdc_keyboard_desc.h"
#include "BASE.h"

#include "USB_User.h"
#include "usbd_int.h"


otg_core_type otg_core_struct;      //  USB全局控制量

#ifdef Exist_USB
static void usb_low_power_wakeup_config(void);
static void usb_clock48m_select(usb_clk48_s clk_s);
static void usb_gpio_config(void);
static void usb_low_power_wakeup_config(void);
#endif

void USB_User_init (void)  
{
#ifdef Exist_USB
    usb_gpio_config();
    usb_low_power_wakeup_config();

    /* enable otgfs clock */
    crm_periph_clock_enable(OTG_CLOCK, TRUE);
    /* select usb 48m clcok source */
    usb_clock48m_select(USB_CLK_HEXT);

    /* enable otgfs irq */
    nvic_irq_enable(OTG_IRQ, 0, 0);
    otg_core_struct.cfg.vbusig = 1;       //关掉PA9
    /* init usb */
    usbd_init(&otg_core_struct,
            USB_FULL_SPEED_CORE_ID,
            USB_ID,
            &cdc_keyboard_class_handler,
            &cdc_keyboard_desc_handler);
    usb_delay_ms(500);
#endif
}

uint16_t USB_Buffer_Receive (uint8_t *Data)
{
#ifdef Exist_USB
    uint16_t len = 0;
    len = usb_Data_get_rxdata(&otg_core_struct.dev, Data);
    return len;
#else 
    return 0;
    
#endif
}

uint16_t USB_Buffer_send (const void *Data,uint16_t *bufflen)
{
#ifdef Exist_USB
    uint16_t Buff_MAX = 64;
    uint8_t Buffer[64];
    uint16_t temp = *bufflen,temp2 = 0;
    usbd_core_type *pudev = &otg_core_struct.dev; 
    
    HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata;      //USB-HID
    do{
        if((temp - Buff_MAX) > 0)
        {
            memcpy(Buffer,(uint8_t*)Data + temp2,Buff_MAX);
            temp -= Buff_MAX;
            temp2 += Buff_MAX;
        }
        else
        {
            memcpy(Buffer,(uint8_t*)Data + temp2,temp);
            temp = 0;
        }
        while(1)
        {
            if(HID->g_custom_tx_completed == 1)       //等上一个发送结束
            {
                HID->g_custom_tx_completed = 0;
                custom_hid_class_send_report(pudev,Buffer,Buff_MAX); //!!!! 这是发送函数本体
                break;
            }
        }
        
        memset(Buffer,0,Buff_MAX);
    }while(temp > 0);

    return 0;
#else 
    return 1;
#endif
    
}

void keyboard_send_string(uint8_t *string, uint8_t len)
{
#ifdef Exist_USB
    uint8_t index = 0;
    usbd_core_type *pudev = &otg_core_struct.dev; 
    HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata;
    for(index = 0; index < len; index ++)
    {
        while(1)
        {
            if(HID->g_keyboard_tx_completed == 1)
            {
                HID->g_keyboard_tx_completed = 0;
                usb_vcpkybrd_keyboard_send_char(&otg_core_struct.dev, string[index]);
                break;
            }
        }
        /* send 0x00 */
        while(1)
        {
            if(HID->g_keyboard_tx_completed == 1)
            {
                HID->g_keyboard_tx_completed = 0;
                usb_vcpkybrd_keyboard_send_char(&otg_core_struct.dev, 0x00);
                break;
            }
        }
    }
#endif
}


uint16_t USB_Keyboard_Send_Data (uint8_t *data, uint16_t u16Sendlen)
{
#ifdef Exist_USB
    uint16_t  i,j,k = 0;
    uint8_t u8SendBuffer[128];        //转换区

    for(i = 0;i < u16Sendlen;i++)
    {
        j = (data[i] >> 4) & 0x0f;
        if(j <= 9)
        {u8SendBuffer[k++] = j + '0'; }
        else
        {u8SendBuffer[k++] = j + ('A' - 0x0a); }
        j = (data[i]) & 0x0f;
        if(j <= 9)
        {u8SendBuffer[k++] = j + '0'; }
        else
        {u8SendBuffer[k++] = j + ('A' - 0x0a); }
    }
    
    keyboard_send_string(u8SendBuffer,k);        //这个不需要缓存区
#endif
    return u16Sendlen;
}


void OTG_IRQ_HANDLER(void)
{
  usbd_irq_handler(&otg_core_struct);
}




void OTG_WKUP_HANDLER(void)
{
    exint_flag_clear(OTG_WKUP_EXINT_LINE);
}


/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
#ifdef USB_LOW_POWER_WAKUP
    exint_init_type exint_init_struct;

    exint_default_para_init(&exint_init_struct);    

    exint_init_struct.line_enable = TRUE;
    exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
    exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
    exint_init(&exint_init_struct);

    nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
#endif
}


/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  switch(system_core_clock)
  {
    /* 48MHz */
    case 48000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1);
      break;

    /* 72MHz */
    case 72000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1_5);
      break;

    /* 96MHz */
    case 96000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2);
      break;

    /* 120MHz */
    case 120000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2_5);
      break;

    /* 144MHz */
    case 144000000:
      crm_usb_clock_div_set(CRM_USB_DIV_3);
      break;

    default:
      break;
  }
}

void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
#endif

  /* otgfs use vbus pin */

  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;      //!!!!
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

}

