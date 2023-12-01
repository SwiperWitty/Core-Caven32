/*
 * gpio_app.h
 *  Feature 功能
 *  Created on: 2023年11月29日
 *
 */

#ifndef USER_GPIO_APP_H_
#define USER_GPIO_APP_H_

#include "Mode.h"

/*
 * GPIO_CMD_SUB
 */
enum APP_GPIO_CMD_SUB
{
    m_GPIO_TEST_Order = 0x00,
    m_GPO_SET_Order,        // GPO控制

    m_GPI_GET_Order,

    m_LED_Status_Order,     // LED控制
    m_BZZ_Status_Order,     // BZZ控制

};

typedef struct
{
    u8 LED_Status_Set[4];
    u8 GPO_Status_Set[36];
    u16 GPO_Keep_Time[36];

    u8 GPI_Status_Get[36];
    u8 GPI_Statls_Last[36];

}GPIO_cfg_Type;

Caven_info_packet_Type gpio_handle(Caven_info_packet_Type data);

#endif /* USER_GPIO_APP_H_ */
