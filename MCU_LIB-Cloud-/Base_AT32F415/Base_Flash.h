#ifndef __Exist_FLASH__H_
#define __Exist_FLASH__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif


/*

    BootLoader
    --empty--(END)
    
    APP(主)              <-    
    --empty--(END)
    
    Data                 <-   
    FLASH_END
    
*/
#ifdef Exist_FLASH
    #define FLASH_PAGE_SIZE (0)                     //一页的大小
    #define FLASH_AREA_SIZE (2*1024)                //区大小
    #define FLASH_SIZE      (128*FLASH_AREA_SIZE)   //Flash大小128区（256k）

    #define FLASH_START     ((uint32_t)0x08000000)  //0x0800 0000
//BootLoader
    #define FLASH_BOOT_SIZE (14 * FLASH_AREA_SIZE)  //28k(BootLoader占用)
    
//APP
    #define FLASH_APP       (FLASH_START + FLASH_APP_SIZE)  //Flash存放【APP的代码区】
    #define FLASH_APP_SIZE  (100 * FLASH_AREA_SIZE)         //
    #define FLASH_APP_END   (FLASH_APP+FLASH_APP_SIZE)      //Flash存放【APP的代码区】结束
    
//Data
    #define FLASH_DATA      FLASH_APP_END               //Flash存放【掉电保存数据】
    #define FLASH_DATA_END  (FLASH_START + FLASH_SIZE)  //Flash存放【掉电保存数据】一直到最大Flash

    #define FLASH_END       FLASH_DATA_END              //Flash最大Flash
#endif

#define Fast_R08_Flash(x)   *(__IO uint8_t*)(x)
#define Fast_R16_Flash(x)   *(__IO uint16_t*)(x)


char Flash_Clear_Area(char Area_Start,char Area_End);

char Flash_Read_Data (int Addr,void *Data,int Lenght);
char Flash_Save_Data (int Addr,const uint16_t *Data,int Lenght);


#endif
