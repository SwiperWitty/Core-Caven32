#include "Flash.h"
#include "string.h"

volatile FLASH_Status EraseStatus,SaveStatus = 0;   //擦除状态、保存状态

int Read_Flash(int Address)
{
#ifdef Exist_FLASH
    int temp;
    volatile FLASH_Status FLASHStatus = 0;
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock();     //普通锁_ON
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

    temp = *(__IO uint16_t*) Address;

    FLASH_Lock();       //普通锁_OFF
    FLASHStatus = FLASH_GetStatus();
    while(FLASHStatus != FLASH_COMPLETE);       //等上一个状态完成
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();
    return temp;
#endif
}

/*
 * 清除256 byte只能使用快速的，快速的是不会有返回值的
 */
char Clear_Flash_Page(int Addr)
{
    char Status = 0;
#ifdef Exist_FLASH
    volatile FLASH_Status FLASHStatus = 0;
    int Address;
    int Page = (Addr -0x08000000) / FLASH_PAGE_SIZE;
    Address = 0x08000000 + (Page * FLASH_PAGE_SIZE);        //本页首地址(区倍数)

//    printf("Address %x \r\n",Address);    //debug

    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock_Fast();     //快速锁_ON
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

    if (Address >= FLASH_DATA && Address < FLASH_END)   //防止过头
    {
        FLASH_ErasePage_Fast(Address);   //擦256
    }

    FLASH_Lock_Fast();      //快速锁_OFF
    FLASHStatus = FLASH_GetStatus();
    while(FLASHStatus != FLASH_COMPLETE);       //等上一个状态完成
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();

    Status = 1;
#endif
    EraseStatus = 0;
    return Status;
}

char Clear_Flash_Area(int addr_start,int addr_end)
{
    char Status = 0;
#ifdef Exist_FLASH
    volatile FLASH_Status FLASHStatus = 0;
    int Address;
    int addr[2];
    int Area = (addr_start -0x08000000) / FLASH_AREA_SIZE;
    addr[0] = 0x08000000 + (Area * FLASH_AREA_SIZE);        //本页首地址(区倍数)
    Area = (addr_end -0x08000000) / FLASH_AREA_SIZE;
    if((addr_end -0x08000000) % FLASH_AREA_SIZE > 0)
            Area++;
    addr[1] = 0x08000000 + (Area * FLASH_AREA_SIZE);        //本页尾地址(区倍数)

//    printf("Area_end %d \r\n",Area);    //debug
//    if (Area > 16) {
//        printf("ch32 over !!\r\n");
//    }

    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock();          //普通锁_ON
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

    if (Area <= 16)
    {
        int temp = 0;
        do {
            Address = addr[0] + temp * FLASH_AREA_SIZE;
            if (Address >= FLASH_DATA && Address <= FLASH_END)  //防止过头
            {
                EraseStatus |= FLASH_ErasePage(Address);        //擦4K
            }
            temp++;
        } while (temp <= ((addr[1] - addr[0]) / FLASH_AREA_SIZE) && (EraseStatus == FLASH_COMPLETE));
    }
    else
    {
        EraseStatus |= FLASH_EraseAllPages();
    }

//    重置内存数据    //debug
//    Address = addr[0];
//    while((Address < addr[1]))
//    {
//        FLASH_ProgramHalfWord(Address, 0xaaaa);
//        Address += 2;
//    }

    FLASH_Lock();                //普通锁_OFF
    FLASHStatus = FLASH_GetStatus();
    while(FLASHStatus != FLASH_COMPLETE);       //等上一个状态完成

    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();

    if(EraseStatus != FLASH_COMPLETE)
    {
        printf("FLASH Erase Fail\r\n");
    }
    else
    {
        printf("FLASH Erase Suc\r\n");
        Status = 1;
    }

#endif
    EraseStatus = 0;
    return Status;
}

/*
 * 清除256 byte只能使用快速的，快速的是不会有返回值的
 * 写数据也没有返回值
 * 但是会有效验
 * Save_Flash是严格按照页来储存的
 * Addr :0X00A06    程序会从 0x00A00区域起，到0x00B00止（严格按页），未写的区域会备份，相当于需要的区域【 &0x00】 再 【|data】
 * Data ：是数据的指针
 * Lenght ：是指针偏移的极限位（偏移结果不应大于止地址，否则保存失败）
 */
char Save_Flash(int Addr,const uint16_t *Data,int Lenght)
{
    char Status = 0;
#ifdef Exist_FLASH
    volatile FLASH_Status FLASHStatus = 0;
    int temp = 0;
    int Address,Address_End;
    int Page = (Addr - 0x08000000) / FLASH_PAGE_SIZE;

    Address = 0x08000000 + (Page * FLASH_PAGE_SIZE);        //本页首地址(区倍数)
    Address_End = Address + FLASH_PAGE_SIZE;
    if ((Lenght + Addr) > Address_End) {                    //超长
        Status = 0;         //仅覆盖一页，超的部分不管
    }
//    printf("Address %x,now: %x \r\n",Address,Addr);     //debug

    uint16_t string[130];
    memcpy(string,(uint16_t *)Address,FLASH_PAGE_SIZE); //备份
    temp = (Lenght < FLASH_PAGE_SIZE)? Lenght : FLASH_PAGE_SIZE;
    memcpy(string + (Addr-Address)/2,Data,temp);        //载入
//    printf("string_start: [%d] %x \n",0,string[0]);
//    printf("string [%d]: %x \n",(Addr-Address)/2,string[(Addr-Address)/2]);
//    printf("string_end [%d]: %x \r\n",(FLASH_PAGE_SIZE-1)/2,string[(FLASH_PAGE_SIZE-1)/2]);
//    int i = 10000;
//    while(i--);

    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;      //降频
    __disable_irq();                            //关中断
    FLASH_Unlock_Fast();        //快速模式_ON
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

    if (Address >= FLASH_DATA && Address < FLASH_END)   //防止过头
    {
        FLASH_ErasePage_Fast(Address);                      //擦256
        FLASH_ProgramPage_Fast(Address,(uint32_t *)string); //写256
    }
    FLASH_Lock_Fast();     //快速模式_OFF
    FLASHStatus = FLASH_GetStatus();
    while(FLASHStatus != FLASH_COMPLETE);       //等上一个状态完成
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();

    int num = 0;
    __IO uint16_t data;
    temp = Address;
    SaveStatus = PASSED;
    while(temp < Address_End)                   //校验
    {
        data = Fast_Flash(temp);
//        printf("flash addr: %x,data_r: %x \n",temp,data);
//        printf("num :%d ,data :%x \n",num,string[num]);
        if(data != string[num])
        {
            SaveStatus = FAILED;
            break;
        }
        temp += 2;
        num += 1;
    }

    if(SaveStatus == FAILED)
    {
//        printf("FLASH Save Fail addr:%x flash:%x--str:%x \n",temp,Fast_Flash(temp),string[num]);
        Status = 0;
    }
    else {
//        printf("ok !\r\n");
        Status = 1;
    }

#endif
    EraseStatus = 0;
    return Status;
}


/*
 * 大致分为以下几步
 * 准备数据处理（栈内存备份Flash数据、确认内存区...）
 * 1.降频
 * 2.关中断
 * 3.解锁-
 * 4.擦除
 * 5.写入
 * 6.锁定-
 * 7.开中断
 * 8.倍频
 * 9.验证
 *
 * 不会有人降频之后再做数据处理吧....
 */



