/*
 * center_app.h
 *
 *  Created on: 2023Äê11ÔÂ29ÈÕ
 */

#ifndef USER_CENTER_APP_H_
#define USER_CENTER_APP_H_

#include "system_app.h"
#include "gpio_app.h"
#include "API.h"

/*
 * CAVEN_CMD
 */
enum CAVEN_CMD
{
    m_CAVEN_TEST_Order = 0x00,
    m_CAVEN_SYS_Order,
    m_CAVEN_BOOT_Order,
    m_CAVEN_FEATURE_Order,
};

/*
 * CAVEN_Status and event
 */
enum CAVEN_STATUS
{
    m_CAVEN_IDLE_Stat = 0,
    m_CAVEN_CHAOS_Stat,
    m_CAVEN_COMM_Stat,
    m_CAVEN_BUSY_Stat,
    m_CAVEN_CHOKE_Stat,
    m_CAVEN_DOWN_Stat,
};

typedef struct
{
    int Status_flag;
    u8 aa;

}CAVEN_Status_Event_Type;

int center_Init(void);
int center_State_machine (Caven_Watch_Type time);

#endif /* USER_CENTER_APP_H_ */
