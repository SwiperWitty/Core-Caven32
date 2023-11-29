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
enum GPIO_CMD_SUB
{
    m_GPIO_TEST_Order = 0x00,
    m_GPO_SET_Order,

    m_GPI_GET_Order,

    m_LED_Status_Order,
    m_BZZ_Status_Order,

};

typedef struct
{
    u8 LED_Status_Set[3];
    u8 GPO_Status_Set[32];
    u32 GPO_Time_Set[32];

    u8 GPI_Status_Get[32];
    u8 GPI_Statls_Last[32];

}GPIO_cfg_Type;

Caven_info_packet_Type gpio_handle(Caven_info_packet_Type data);

#endif /* USER_GPIO_APP_H_ */
