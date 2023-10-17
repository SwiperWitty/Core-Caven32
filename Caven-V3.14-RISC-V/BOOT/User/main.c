#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_prop.h"

#include "API.h"
#include "Mode.h"

#include "Agreement_Hanlde.h"

#define GO_TO_APP() do{     \
    __asm("li  a6, 0x6000");\
    __asm("jr  a6");        \
    while(1);               \
}while(0);

#define Soft_Version 0X5A01

extern uint8_t HIDTxBuffer[];
extern struct Caven_Data HID_Data;      //USBԴ

struct _Agreement USB_Pack;
struct _Agreement UART_Pack;
struct _Agreement Send_DATA;
struct _Function_ Fun;

int led;
int usb;

void Main_Init (void);
char Send_Data(const U8 *Data, int Length);
int main(void)
{
    Main_Init();
    char Free_array[50];
    struct _Over_time LED_F = {
       .Now_Time = &SYS_Time.Watch,
       .Now_data = &led,
       .Set_Time.second = 1,
       .Set_Time.time_us = 200000,
    };
    struct _Over_time USB_F = {
       .Now_Time = &SYS_Time.Watch,
       .Now_data = &usb,
       .Set_Time.second = 1,
       .Set_Time.time_us = 0,
    };
    uint16_t str_num = 0;
    int Down_num = 0;
    while(1)
    {
        SYS_Time.Watch = Get_TIME ();
        Pick_Agreement(&HID_Data, &USB_Pack, &Fun,0);           //HID_Data.DATA/HID_Data
        Pick_Agreement(&CV_UART[1].DATA, &UART_Pack, &Fun,0);   //UART

        if (USB_Pack.Flag == 'F')                   //usb����
        {
            U8 Data_temp[1024] = {0};
            if (USB_Pack.Error.Flag) {
                Data_temp[0] = USB_Pack.Error.E_Class;
                Data_temp[1] = 0x00;
                Data_temp[2] = 0x00;
                Data_temp[3] = 0x00;
                Data_temp[4] = (U8)((USB_Pack.Length >> 8) & 0x00ff);
                Data_temp[5] = (U8)(USB_Pack.Length & 0x00ff);
                Send_DATA.Length = 6;
                Send_DATA.Head = Agreement_Head;
                Send_DATA.pointer = Data_temp;
                Send_DATA.Con = 0x00010000;
            }
            else
            {
                if (USB_Pack.Control.Class == 1)        //���������Class
                {
                    switch (USB_Pack.Control.MID)   //�鿴��λ������ʲô
                    {
                    case 0:         //��ѯ��д����Ϣ
                        Send_DATA.Length = 0x00;
                        break;
                    case (0x01):    //�鿴�汾��Ϣ
                        Data_temp[0] = 0x01;
                        Data_temp[1] = 0x01;
                        Data_temp[2] = 0x07;
                        Data_temp[3] = 0x06;
                        Send_DATA.Length = 4;
                        break;
                    case (0x02):             //���ò����ʣ��������ò����ʣ�
                        Data_temp[0] = 0x01; //����ʧ��
                        Send_DATA.Length = 1;
                        break;
                    case (0x03):    //��ѯ������
                        Data_temp[0] = 0x02;        //115200
                        Send_DATA.Length = 1;
                        break;
                    case (0x0F):    //��λ
                        Debug_Out("Reset !\r\n \r\n\r\n");
                        NVIC_SystemReset();
                        break;
                        Data_temp[0] = 0x00; // ���� �ɹ�
                        Send_DATA.Length = 1;
                        break;
                    default:
                        break;
                    }
                    Send_DATA.Con = 0x00010100 + USB_Pack.Control.MID;
                }
                else if (USB_Pack.Control.Class == 2)   //RFID���������Class
                {
                    Send_DATA.Con = 0x00010200 + USB_Pack.Control.MID;
                }
                else if (USB_Pack.Control.Class == 3)   //��д����־
                {
                    Send_DATA.Con = 0x00010300 + USB_Pack.Control.MID;
                }
                else if (USB_Pack.Control.Class == 4)
                {
                    switch(USB_Pack.Control.MID)
                    {
                        case 0:                     //Ӧ������
                            Send_DATA.Length = 5;
                            memcpy(Data_temp,USB_Pack.Buff,Send_DATA.Length);
                            Data_temp[4] = 1;       //����ʧ��
                            break;
                        case 1:                     //��������
                            if (USB_Pack.Buff[0] == 0xff) {     //����
                                uint16_t Serial[4] = {Soft_Version,0xffff};     //���������汾 & Ĭ�ϱ���DATA
                                Serial[2] = Down_num >> 16;
                                Serial[3] = Down_num & 0x0000ffff;              //���򳤶�
                                Save_Flash(FLASH_DATA,Serial,sizeof(Serial));   //���籣������ֵ&����
//                                printf(" %d !\r\n",Down_num);
//                                printf("updata over !\r\n");
                            }
                            else {
                                memset(Data_temp,0xff,0x200);                           //�ڴ�����
                                str_num = (USB_Pack.Buff[4] << 8) + USB_Pack.Buff[5];   //��ֻ����APP����ʱ��������
                                memcpy(Data_temp,(USB_Pack.Buff + 6),str_num);          //��Data_tempΪ���棬�հ���0XFF
                                Save_Flash(FLASH_CODE + Down_num,(uint16_t *)Data_temp,FLASH_PAGE_SIZE);  //��ʼдFlash(256)
                                Down_num += str_num;                //����APP�ĵ�ǰ���ȣ�ϣ������256����������
                            }

                            Send_DATA.Length = 5;
                            memcpy(Data_temp,USB_Pack.Buff,Send_DATA.Length);
                            Data_temp[4] = 0;       //�����ɹ�
                            break;
                        default:
                            break;
                    }
                    Send_DATA.Con = 0x00010400 + USB_Pack.Control.MID;
                }
                Send_DATA.Head = Agreement_Head;
                Send_DATA.pointer = Data_temp;
            }
            usb++;
            Send_Agreement(&Send_DATA, &Fun);       //����Ϣ(bug)
            Destroy(&Send_DATA, sizeof(Send_DATA)); //����
            Destroy(&USB_Pack, sizeof(USB_Pack));
        }
        if (UART_Pack.Flag == 'F')                   //UART����
        {
            U8 Data_temp[1024] = {0};
            if (UART_Pack.Error.Flag) {
                Data_temp[0] = UART_Pack.Error.E_Class;
                Data_temp[1] = 0x00;
                Data_temp[2] = 0x00;
                Data_temp[3] = 0x00;
                Data_temp[4] = (U8)((UART_Pack.Length >> 8) & 0x00ff);
                Data_temp[5] = (U8)(UART_Pack.Length & 0x00ff);
                Send_DATA.Length = 6;
                Send_DATA.Head = Agreement_Head;
                Send_DATA.pointer = Data_temp;
                Send_DATA.Con = 0x00010000;
            }
            else
            {
                if (UART_Pack.Control.Class == 1)        //���������Class
                {
                    switch (UART_Pack.Control.MID)   //�鿴��λ������ʲô
                    {
                    case 0:         //��ѯ��д����Ϣ
                        Send_DATA.Length = 0x00;
                        break;
                    case (0x01):    //�鿴�汾��Ϣ
                        Data_temp[0] = 0x01;
                        Data_temp[1] = 0x01;
                        Data_temp[2] = 0x07;
                        Data_temp[3] = 0x06;
                        Send_DATA.Length = 4;
                        break;
                    case (0x02):             //���ò����ʣ��������ò����ʣ�
                        Data_temp[0] = 0x01; //����ʧ��
                        Send_DATA.Length = 1;
                        break;
                    case (0x03):    //��ѯ������
                        Data_temp[0] = 0x02;        //115200
                        Send_DATA.Length = 1;
                        break;
                    case (0x0F):    //��λ
                        Debug_Out("Reset !\r\n \r\n\r\n");
                        NVIC_SystemReset();
                        break;
                        Data_temp[0] = 0x00; // ���� �ɹ�
                        Send_DATA.Length = 1;
                        break;
                    default:
                        break;
                    }
                    Send_DATA.Con = 0x00010100 + UART_Pack.Control.MID;
                }
                else if (UART_Pack.Control.Class == 2)   //RFID���������Class
                {
                    Send_DATA.Con = 0x00010200 + UART_Pack.Control.MID;
                }
                else if (UART_Pack.Control.Class == 3)   //��д����־
                {
                    Send_DATA.Con = 0x00010300 + UART_Pack.Control.MID;
                }
                else if (UART_Pack.Control.Class == 4)
                {
                    switch(UART_Pack.Control.MID)
                    {
                        case 0:                     //Ӧ������
                            Send_DATA.Length = 5;
                            memcpy(Data_temp,UART_Pack.Buff,Send_DATA.Length);
                            Data_temp[4] = 1;       //����ʧ��
                            break;
                        case 1:                     //��������
                            if (UART_Pack.Buff[0] == 0xff) {     //����
                                uint16_t Serial[4] = {Soft_Version,0xffff};     //���������汾 & Ĭ�ϱ���DATA
                                Serial[2] = Down_num >> 16;
                                Serial[3] = Down_num & 0x0000ffff;              //���򳤶�
                                Save_Flash(FLASH_DATA,Serial,sizeof(Serial));   //���籣������ֵ&����
//                                printf(" %d !\r\n",Down_num);
//                                printf("updata over !\r\n");
                            }
                            else {
                                memset(Data_temp,0xff,0x200);                           //�ڴ�����
                                str_num = (UART_Pack.Buff[4] << 8) + UART_Pack.Buff[5]; //��ֻ����APP����ʱ��������
                                memcpy(Data_temp,(UART_Pack.Buff + 6),str_num);         //��Data_tempΪ���棬�հ���0XFF
                                Save_Flash(FLASH_CODE + Down_num, (uint16_t *)Data_temp,FLASH_PAGE_SIZE);  //��ʼдFlash(256)
                                Down_num += str_num;                //����APP�ĵ�ǰ���ȣ�ϣ������256����������
                            }
                            Send_DATA.Length = 5;
                            memcpy(Data_temp,UART_Pack.Buff,Send_DATA.Length);
                            Data_temp[4] = 0;       //�����ɹ�
                            break;
                        default:
                            break;
                    }
                    Send_DATA.Con = 0x00010400 + UART_Pack.Control.MID;
                }
                Send_DATA.Head = Agreement_Head;
                Send_DATA.pointer = Data_temp;
            }
            usb++;
            Send_Agreement(&Send_DATA, &Fun);       //����Ϣ(bug)
            Destroy(&Send_DATA, sizeof(Send_DATA)); //����
            Destroy(&UART_Pack, sizeof(UART_Pack));
        }
        sprintf(Free_array,"Time: %2d:%2d:%2d \r\n",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);

        API.Time.Over_Time(&USB_F);
        API.Time.Over_Time(&LED_F);         //LED��˸
        if(LED_F.Flip)
        {
            Mode_Use.LED.LED_SET(2,ENABLE);
        }
        else
        {
            Mode_Use.LED.LED_SET(2,DISABLE);
        }
        if (USB_F.Flag) {
            USB_F.Flag = 0;
//            printf("Time: %2d:%2d:%2d \r\n",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);
        }
//        Mode_Use.Delay.Delay_ms(700);
//        Mode_Use.LED.LED_SET(1,DISABLE);
//        Mode_Use.Delay.Delay_ms(700);
//        Mode_Use.LED.LED_SET(1,ENABLE);

    }
}


char Send_Data(const U8 *Data, int Length)      //USB
{
    char bk = 0;
    int temp= Length;
    int num = 0;
    if (USB_Pack.Flag)
    {
        do {
            for(int i = 0;i< ENDP2_IN_SIZE;i++) HIDTxBuffer[i] = 0;
            if(temp >= 64)
            {
                for(int i = 0;i< 64;i++) HIDTxBuffer[i] = Data[num + i];
                temp -= 64;
                num += 64;
            }
            else {
                for(int i = 0;i< temp;i++) HIDTxBuffer[i] = Data[num + i];
                temp = 0;
            }
            USBD_HID_Data_Updata();                         //��usb
        } while (temp);
    }
    if (UART_Pack.Flag)
    {
        Mode_Use.UART.Send_Data(1,Data,temp);              //������
    }
    return bk;
}

void Main_Init (void)
{
    //Bootloard��ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();               //!!
    API_Index();

    Mode_Init.UART(1,115200,ENABLE);
    Mode_Init.TIME(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.BZZ(ENABLE);
    Mode_Use.LED.LED_SET(1,ENABLE);
    Mode_Use.LED.LED_SET(2,DISABLE);
    Mode_Use.BZZ.BZZ_SET(DISABLE);
    Mode_Use.UART.Send_String(1,"Start BootLoader \r\n");

    printf("Soft_Version : %x \r\n",Fast_Flash(FLASH_DATA));
    if (Fast_Flash(FLASH_DATA) == Soft_Version) {
        int end = (Fast_Flash(FLASH_DATA + 4) << 16) + Fast_Flash(FLASH_DATA + 6);
        printf("Flash-test: lenght :%d  \r\n",end);
        printf("Flash-test: addr :%x -- data :%x \r\n",FLASH_CODE,Fast_Flash(FLASH_CODE));
        printf("Flash-test: addr :%x -- data :%x \r\n",FLASH_CODE + 12,Fast_Flash(FLASH_CODE + 12));
        printf("Flash-test: addr :%x -- data :%x \r\n",(FLASH_CODE + end + 2),Fast_8_Flash(FLASH_CODE + end));

        Mode_Use.UART.Send_String(1,"Jump to APP \r\n\r\n");
        Mode_Use.LED.LED_SET(1,DISABLE);
        Mode_Use.LED.LED_SET(2,ENABLE);
        Mode_Init.TIME(DISABLE);                //ǿ�ƹرյδ�ʱ��
//        __disable_irq();
        GO_TO_APP();
    }

    //USB��ʼ��
    Set_USBConfig();
    USB_Init();
    USB_Port_Set(DISABLE, DISABLE);
    Mode_Use.Delay.Delay_ms(700);
    USB_Port_Set(ENABLE, ENABLE);
    USB_Interrupts_Config();

    Mode_Use.Delay.Delay_ms(700);

    //�����ǽ�Э���õĺ���������
    Fun.Delay = Mode_Use.Delay.Delay_ms;
    Fun.Now_Time = &SYS_Time.Watch;
    Fun.Over_Time = Over_Time;
    Fun.Debug_Out = Mode_Use.Debug_Out;
    Fun.Result_Code = CRC16_CCITT_CalculateBuf;
    Fun.Send_Data = Send_Data;

    printf("go \r\n");
    Set_TIME (SYS_Time.Watch);
}


