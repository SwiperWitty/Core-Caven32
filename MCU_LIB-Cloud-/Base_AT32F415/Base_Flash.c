#include "Base_Flash.h"

#include "Caven.h"              //MAX,MIN




char Flash_Addr_Check(int Addr_Start,int Addr_End)
{
	
    flash_status_type back = FLASH_OPERATE_DONE;
#ifdef Exist_FLASH 
    if((Addr_Start >= FLASH_END) || (Addr_Start < FLASH_START))
    { 
        printf("S-ERROR\r\n"); 
        back = FLASH_PROGRAM_ERROR;
        return back;
    }
    if((Addr_End > FLASH_END)  || (Addr_End <= FLASH_START))
    { 
        printf("E-ERROR\r\n"); 
        back = FLASH_PROGRAM_ERROR;
        return back;
    }
    if(Addr_Start > FLASH_END)
    {
        back = FLASH_PROGRAM_ERROR;
        return back;
    }
    printf("ch- p: [S - %d,E -%d) \r\n",(Addr_Start-FLASH_START)/FLASH_AREA_SIZE,(Addr_End-FLASH_START)/FLASH_AREA_SIZE);
#endif
    return back;
}

char Flash_Clear_Area(char Area_Start,char Area_End)
{
    char status = FLASH_OPERATE_DONE;
#ifdef Exist_FLASH 
    status = Flash_Addr_Check(GET_Area_Addr(Area_Start),GET_Area_Addr(Area_End));
    if(status == FLASH_PROGRAM_ERROR) {return ERROR;}
//    printf("clr- p: [S_Area %d , E_Area %d) \r\n",Area_Start,Area_End);
    
    flash_unlock();
    for(int i = Area_Start;i <= Area_End;i++)
    {
        /* wait for operation to be completed */
        status = flash_operation_wait_for(ERASE_TIMEOUT);
        if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
        else if(status == FLASH_OPERATE_TIMEOUT)
        {return ERROR;}
        
        //Clear
        status = flash_sector_erase(GET_Area_Addr(i));
        if(status != FLASH_OPERATE_DONE)
        {
//            printf("p: Clr ERROR \r\n");
            return ERROR;
        }
        else
        {
//            printf("p: Clr secc %d \r\n",i);
        }
    }
    flash_lock();
#endif
    return status;
}


char Flash_Save_Area (void * Data,char Area_Start,int Lenght)
{
    char back = SUCCESS;
#ifdef Exist_FLASH 
    char Buff[FLASH_AREA_SIZE];         //缓冲区      而且要
    char status = FLASH_OPERATE_DONE;
    memcpy(Buff,Data,Lenght);           //向前对齐
    status = Flash_Addr_Check(GET_Area_Addr(Area_Start),GET_Area_Addr(Area_Start));
    if(status == FLASH_PROGRAM_ERROR) {return ERROR;}
    
    flash_unlock();
    status = flash_operation_wait_for(ERASE_TIMEOUT);
    if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
    {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
    else if(status == FLASH_OPERATE_TIMEOUT)
    {return ERROR;}
    
    status = flash_sector_erase(GET_Area_Addr(Area_Start));         //清除一区
    if(status != FLASH_OPERATE_DONE)
    {return ERROR;}
    
    for(int i = 0;i < FLASH_AREA_SIZE;i++)                          //保存
    {
        status = flash_byte_program(GET_Area_Addr(Area_Start)+i, Buff[i]);
        if(status !=  FLASH_OPERATE_DONE)
        {return ERROR;}
    }
    
    flash_lock();
#endif
    return back;
}

char Flash_Save_Data(int Addr,const uint16_t *Data,int Lenght)
{
	char back = SUCCESS;
#ifdef Exist_FLASH 
    uint16_t flash_buf[FLASH_AREA_SIZE];        //缓冲区      而且要
    char status = FLASH_OPERATE_DONE;
    uint32_t Temp;
    
    if(Addr%2)  Addr++;
    int Addr_Start = Addr;          
    int Addr_End = Addr + Lenght;   //
    
    status = Flash_Addr_Check(Addr_Start,Addr_End); //11
    if(status == FLASH_PROGRAM_ERROR)
    {return ERROR; }
    
    Temp = (Addr_Start - FLASH_START) / FLASH_AREA_SIZE;
    char Area_Start = Temp;                     //首页 
    
    Temp = ((Addr_End - FLASH_START) / FLASH_AREA_SIZE);
    if(Temp * FLASH_AREA_SIZE < Addr_End)
    {Temp++;}
    char Area_End = Temp;          //尾页 

    int Sour_NUM = 0;               //源(Data)
    int Targ_NUM = 0;               //缓冲区
    int Cpy_num = 0;                //复制数
    
    for(int i = Area_Start;i < Area_End;i++)
    {
        //开始数据处理
        Temp = GET_Area_Addr(i);
        back = Flash_Read_Data (Temp,flash_buf,FLASH_AREA_SIZE);     //Flash备份
        if(back == 0) 
        {return ERROR;}
        
        Targ_NUM = 0;
        if(Temp < Addr_Start)        //第一页  覆盖数据
        {
            Targ_NUM = (Addr_Start - Temp);
            Cpy_num = FLASH_AREA_SIZE - Targ_NUM;
            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
//            printf("1-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);

            Sour_NUM += Cpy_num; 
        }
        else if(GET_Area_Addr(i+1) > Addr_End)        //最后一页
        {
            Cpy_num = Addr_End - Temp;
            if(Cpy_num == 0)
            {return SUCCESS;}              //结束
            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
//            printf("2-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);
            
//            Sour_NUM += Temp;
        }
        else
        {
            Cpy_num = FLASH_AREA_SIZE;
            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
//            printf("3-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);
            
            Sour_NUM += Cpy_num;
        }
        status = flash_operation_wait_for(ERASE_TIMEOUT);
        if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
        else if(status == FLASH_OPERATE_TIMEOUT)
        {return ERROR; }
        
        back = Flash_Clear_Area(i,i + 1);                                   //擦一页
        if(back == 0) 
        {return ERROR; } 
//        printf("AREA : %d  Flash : %x ; wirte size : %x\r\n",i,GET_Area_Addr(i),FLASH_AREA_SIZE);
        
        flash_unlock();
        
        for(int i = 0;i < FLASH_AREA_SIZE/2;i++)        //覆盖保存数据
        {
//            printf("%x ",flash_buf[i]);
            status = flash_halfword_program(Temp, flash_buf[i]);    

            if(status == FLASH_PROGRAM_ERROR)
            {   
//                printf("W ERROR \r\n ");
                return ERROR; 
            }
            
            flash_buf[i] = 0;
            Temp += 2;
        }
        flash_lock();

    }
#endif
    return back;
}


char Flash_Read_Data (int Addr,void *Data,int Lenght)
{
	char back = SUCCESS;
#ifdef Exist_FLASH 
    if(Addr%2)  {Addr++;}       //首地址最好不要是单数
    back = Flash_Addr_Check(Addr,(Addr + Lenght));
    if(back == ERROR)
    {return ERROR; }
    if(Data == NULL)
    {return ERROR; }
    
//    for(int i;i < Lenght;i += 2)
//    {
//        *((uint16_t*) Data + i) = Fast_R16_Flash(Addr + i);
//        Addr += 2;
//    }

    int * Pointer = (int *)(Addr);

    memcpy(Data,Pointer,Lenght);        //复制,void *memcpy(void *str1, const void *str2, size_t n) 
#endif
    return back;
}

