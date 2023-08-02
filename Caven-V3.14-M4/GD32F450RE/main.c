
#if 1

#include "gd32f4xx.h"
#include <stdio.h>
#include "main.h"
#include "string.h"
#include "gd32f450i_eval.h"

void fmc_init(void)
{
    fmc_unlock();
    ob_unlock();
    ob_write_protection_disable(OB_WP_ALL);
    ob_start();
    ob_lock();
    fmc_lock();
}

/*!
    \brief      gets the sector of a given address
    \param[in]  address: a given address(0x08000000~0x080FFFFF)
    \param[out] none
    \retval     the sector of a given address
*/
uint32_t fmc_sector_get(uint32_t address)
{
    uint32_t sector = 0;
    if((address < ADDR_FMC_SECTOR_1) && (address >= ADDR_FMC_SECTOR_0)){
        sector = CTL_SECTOR_NUMBER_0;
    }else if((address < ADDR_FMC_SECTOR_2) && (address >= ADDR_FMC_SECTOR_1)){
        sector = CTL_SECTOR_NUMBER_1;
    }else if((address < ADDR_FMC_SECTOR_3) && (address >= ADDR_FMC_SECTOR_2)){
        sector = CTL_SECTOR_NUMBER_2;
    }else if((address < ADDR_FMC_SECTOR_4) && (address >= ADDR_FMC_SECTOR_3)){
        sector = CTL_SECTOR_NUMBER_3;  
    }else if((address < ADDR_FMC_SECTOR_5) && (address >= ADDR_FMC_SECTOR_4)){
        sector = CTL_SECTOR_NUMBER_4;
    }else if((address < ADDR_FMC_SECTOR_6) && (address >= ADDR_FMC_SECTOR_5)){
        sector = CTL_SECTOR_NUMBER_5;
    }else if((address < ADDR_FMC_SECTOR_7) && (address >= ADDR_FMC_SECTOR_6)){
        sector = CTL_SECTOR_NUMBER_6;
    }else if((address < ADDR_FMC_SECTOR_8) && (address >= ADDR_FMC_SECTOR_7)){
        sector = CTL_SECTOR_NUMBER_7;  
    }else if((address < ADDR_FMC_SECTOR_9) && (address >= ADDR_FMC_SECTOR_8)){
        sector = CTL_SECTOR_NUMBER_8;
    }else if((address < ADDR_FMC_SECTOR_10) && (address >= ADDR_FMC_SECTOR_9)){
        sector = CTL_SECTOR_NUMBER_9;
    }else if((address < ADDR_FMC_SECTOR_11) && (address >= ADDR_FMC_SECTOR_10)){
        sector = CTL_SECTOR_NUMBER_10;
    }else{
        sector = CTL_SECTOR_NUMBER_11;
    }
    return sector;
}

void fmc_erase_sector(uint32_t fmc_sector)
{
    /* unlock the flash program erase controller */
    fmc_unlock(); 
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* wait the erase operation complete*/
    if(FMC_READY != fmc_sector_erase(fmc_sector)){
        while(1);
    }
    /* lock the flash program erase controller */
    fmc_lock();
}

int fmc_write_data(int address, const void * data,int size)
{
    int retval = 0;
    int StartSector, EndSector,temp;
    int array[0x200];
    int size_array = sizeof(array);
    if(address < 0x08000000 || data == NULL)
    {
        retval = 1;
        return retval;
    }
    
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the number of the start and end sectors */
    StartSector = fmc_sector_get(address);
    EndSector = fmc_sector_get(address + size);
    /* each time the sector number increased by 8, refer to the sector definition */
    for(int i = StartSector; i <= EndSector; i += 8){
        if(FMC_READY != fmc_sector_erase(i)){
            while(1);
        }
    }
    /* write data_32 to the corresponding address */
    for(int i = 0; i < size;)
    {
        if((size - i)/size_array)
        {temp = size_array;}
        else
        {temp = size - i;}
        
        memset(array,0,size_array);
        memcpy(array,(char *)data + i,temp);
        int k = 0;
        for(int j = 0;j < temp;)    //一定会多
        {
            if(FMC_READY == fmc_word_program(address, array[k++]))
            {
                address = address + 4;
                j += 4;
            }else
            { 
                while(1);
            }
        }
        i += temp;
    }
    /* lock the flash program erase controller */
    fmc_lock();
    return retval;
}

int fmc_read_data(int address, void * data,int size)
{
    char temp;
    int retval = 0;
    if(address < 0x08000000 || data == NULL)
    {
        retval = 1;
        return retval;
    }
    for(int i = 0; i < size; i++)
    {
        temp = *((__IO char*)address + i);
        *((char *)data + i) = temp;
    }
    return retval;
}

typedef struct fcm_data_t
{
    int a;
    char b;
    char c[1100];
    char end;
} fcm_data;

fcm_data test_f = {
    .a = 0x0000f0fa,
    .b = 58,
//    .c = "012345",
    .end = 0x5A,
};
fcm_data test_user_r,test_user_1;

#define addr_f (int)0x0803e000
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    memset(&test_user_r,0,sizeof(test_user_r));

    gd_eval_led_init(LED2);
    fmc_init();
    gd_eval_led_off(LED2);
    memset(test_f.c,0,sizeof(test_f.c));
    /* read 32 bit length data from a given address */
    fmc_read_data(addr_f,&test_user_r,sizeof(test_user_r));

    if(test_user_r.end == 0x5A)                             //上电读取
    {
        gd_eval_led_on(LED2);
        test_user_r.end++;
        test_user_r.a = sizeof(fcm_data);
        fmc_write_data(addr_f, &test_user_r,sizeof(test_user_r));
    }
    else
    {
        gd_eval_led_off(LED2);
        fmc_write_data(addr_f, &test_f,sizeof(test_f));     //写入默认
    }

    fmc_read_data(addr_f,&test_user_1,sizeof(test_user_1));

    while(1);
}

#endif
