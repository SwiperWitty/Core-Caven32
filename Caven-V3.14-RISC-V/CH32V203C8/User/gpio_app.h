/*
 * gpio_app.h
 *  Feature 功能
 *  Created on: 2023年11月29日
 *
 */

#ifndef USER_GPIO_APP_H_
#define USER_GPIO_APP_H_

#include "Mode.h"

// event
extern Caven_event_Type SYS_events;
extern int LED_event_Handle;
extern int BZZ_event_Handle;
extern int GPO_event_Handle;


/*
 * GPIO_CMD_SUB
 */
enum APP_GPIO_CMD_SUB
{
    m_GPIO_TEST_Order = 0x00,
    m_GPIO_TRANSPOND_Order, // 消息转发
    m_GPO_SET_Order,        // GPO控制

    m_GPI_GET_Order,

    m_LED_Status_Order,     // LED控制
    m_BZZ_Status_Order,     // BZZ控制

};

typedef struct
{
    u8 GPO_num;
    u8 GPI_num;

    u8 GPO_Status_Set[36];
    u8 GPO_Status_Last[36];
    int GPO_Keep_Time;

    u8 GPI_Status_Get[36];
    u8 GPI_Statls_Last[36];

    u8 LED_Status_Set[4];
    u8 BZZ_Status_Set[2];
    int BZZ_Keep_Time;

}GPIO_cfg_Type;

Caven_info_packet_Type gpio_handle(Caven_info_packet_Type data);

int GPI_Change_Updata_Handle (void);

// event
void LED_event_task_Fun (void * data);
void BZZ_event_task_Fun (void * data);
void GPO_event_task_Fun (void * data);


#endif /* USER_GPIO_APP_H_ */
