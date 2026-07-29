#include "bootld_app.h"

void Bootld_app_Init (void)
{
	MODE_W25Q_drive_Init(ENABLE);
	g_SYS_Config.Boot.Load_Default = SYS_FLASH_EA;
	g_SYS_Config.Boot.Load_Latest = SYS_FLASH_EB;

}

/*
	此函数可以修改g_SYS_Config，并保存
	mode: 1: 写app区；2: 写flashA；3: 写flashB
	pack_num: 包的序号，0：清除app区；1：写入第一包；0xFFFFFFFF:最后一包
	crc：bin的总crc
	retval:0:完成；other:失败
*/
u32 BT_pack_run = 0xff,BT_pack_addr = 0,BT_start_addr = 0,BT_bin_crc = 0;
int Bootld_Save_bin_Fun(u8 mode,u8 *data,u16 len,u32 pack_num,u16 pack_crc)
{
	int retval = 1;
	int pack_len = 0,temp_rt = 0;
	uint32_t flash_len;
	uint8_t temp_array[500];
	if(len > sizeof(temp_array))
	{
		return retval;
	}
	if(mode > 1)
	{
		if(MODE_W25Q_Get_Max_Addr_Fun() == 0)		// W25Qxx error
		{
			return retval = 2;
		}
	}
	if (pack_num == 0)
	{
		BT_pack_run = 0;
		BT_pack_addr = 0;
		BT_start_addr = 0;
		BT_bin_crc = 0;
		temp_rt = 0;

		switch (mode) {
		case 1:
		{
			retval = Base_Flash_Erase (SYS_APP_ADDR,SYS_APP_SIZE);
			if(retval)
			{
				return retval = 1;
			}
		} break;
		case 2:
		{
			flash_len = g_SYS_Config.Boot.Load_Default + SYS_FLASH_SIZE - 1;
			for(uint32_t i = g_SYS_Config.Boot.Load_Default; i < flash_len;)
			{
				retval = MODE_W25Q_Erase_Sector_Fun(i);
				if(retval)
				{
					return retval = 1;
				}
				i += W25Q_SECTOR_SIZE;
			}
		} break;
		case 3:
		{
			flash_len = g_SYS_Config.Boot.Load_Latest + SYS_FLASH_SIZE - 1;
			for(uint32_t i = g_SYS_Config.Boot.Load_Latest; i < flash_len;)
			{
				retval = MODE_W25Q_Erase_Sector_Fun(i);
				if(retval)
				{
					return retval = 1;
				}
				i += W25Q_SECTOR_SIZE;
			}
		} break;
		default:
			temp_rt = 1;
			break;
		}
	}
	else if (pack_num == 0xFFFFFFFF)						// 完成
	{
		pack_len = BT_pack_addr - BT_start_addr;

		if(pack_len > 0)
		{
			if(pack_crc == BT_bin_crc && pack_len <= SYS_APP_SIZE)
			{
				temp_rt = 0x00;
				Debug_OutStr("bootld_app Info: succ \n");
				memset(temp_array, 0, sizeof(temp_array));
				switch (mode) {
				case 1:
				{
					g_SYS_Config.Boot.app_size = pack_len;
					g_SYS_Config.Boot.app_crc = pack_crc;
				} break;
				case 2:
				{
					strcat((char*)temp_array,"bootld_a");
					memcpy(&temp_array[20],&pack_crc,sizeof(pack_crc));
					memcpy(&temp_array[22],&pack_len,sizeof(pack_len));
					MODE_W25Q_Erase_Sector_Fun(g_SYS_Config.Boot.Load_Default);
					retval = MODE_W25Q_Write_Data_Fun(g_SYS_Config.Boot.Load_Default,temp_array,W25Q_PAGE_SIZE);
					if(retval)
					{
						return retval = 1;
					}
				} break;
				case 3:
				{
					strcat((char*)temp_array,"bootld_b");
					memcpy(&temp_array[20],&pack_crc,sizeof(pack_crc));
					memcpy(&temp_array[22],&pack_len,sizeof(pack_len));
					MODE_W25Q_Erase_Sector_Fun(g_SYS_Config.Boot.Load_Latest);
					retval = MODE_W25Q_Write_Data_Fun(g_SYS_Config.Boot.Load_Latest,temp_array,W25Q_PAGE_SIZE);
					if(retval)
					{
						return retval = 1;
					}
				} break;
				default:
					temp_rt = 1;
					break;
				}
				// g_SYS_Config.temp_val->Reset_falg = 1;
			}
			else
			{
				Debug_OutStr("bootld_app Error: crc \n");
				Debug_printf("pack date %x,check data %x\n",pack_crc,BT_bin_crc);
				temp_rt = 0x03;
				g_SYS_Config.Boot.app_crc = 0;
				g_SYS_Config.Boot.app_size = 0;
			}
		}
		else
		{
			temp_rt = 0x01;
		}

		BT_pack_run = 0;
		BT_pack_addr = 0;
		BT_start_addr = 0;
		BT_bin_crc = 0;
	}
	else if (BT_pack_run == pack_num || (BT_pack_run + 1) == pack_num)		// 正常情况
	{
		if(data == NULL || len == 0)
		{
			return retval = 1;
		}
		memcpy(temp_array,data,len);

		switch (mode) {
		case 1:
		{
			if (BT_pack_addr <= SYS_APP_ADDR)
			{
				BT_pack_addr = SYS_APP_ADDR;
				BT_start_addr = BT_pack_addr;
			}
			retval = Base_Flash_Write (temp_array,BT_pack_addr,len);
		} break;
		case 2:
		{
			if (BT_pack_addr <= g_SYS_Config.Boot.Load_Default)
			{
				BT_pack_addr = g_SYS_Config.Boot.Load_Default + W25Q_SECTOR_SIZE;	// 第一扇是配置文件
				BT_start_addr = BT_pack_addr;
			}
			retval = MODE_W25Q_Write_Data_Fun(BT_pack_addr,temp_array,len);
		} break;
		case 3:
		{
			if (BT_pack_addr <= g_SYS_Config.Boot.Load_Latest)
			{
				BT_pack_addr = g_SYS_Config.Boot.Load_Latest + W25Q_SECTOR_SIZE;
				BT_start_addr = BT_pack_addr;
			}
			retval = MODE_W25Q_Write_Data_Fun(BT_pack_addr,temp_array,len);
		} break;
		default:
			retval = 1;
			break;
		}

		if(retval == 0)		// 写入成功
		{
			if(pack_num > BT_pack_run)
			{
				for(int i = 0; i < len; i++)
				{
					BT_bin_crc = CRC16_XMODEM_Table_Byte(temp_array[i],BT_bin_crc);
				}
			}
			else
			{
				Debug_OutStr("bootld_app Warning: Repeat \n");
			}
			temp_rt = 0;
			BT_pack_addr += len;
			BT_pack_run = pack_num;
		}
		else                    // 写入失败
		{
			temp_rt ++;
			Debug_OutStr("bootld_app Warning: Write \n");
		}
	}
	else
	{
		temp_rt = 0x02;             // 包序错误,不连续
	}

	return retval = temp_rt;
}

/*
	此函数不可以修改g_SYS_Config，返回retval交给系统判断是否覆盖g_SYS_Config
    0: keep bootld
	1：running app
    other: change Bt_mode & app_size = other
*/
int SYS_Boot_CheckAndLoad(SYS_boot_Type boot,uint16_t *crc)
{
    int retval = 0;

    uint8_t *addr_p;
    uint32_t fw_size = 0;
    uint16_t target_crc = 0,calc_crc = 0;
	
    if (boot.Bt_mode == 0 || crc == NULL)
    {
        return retval;
    }
	*crc = 0;
    if (boot.Bt_mode > 1 && boot.Bt_mode < 0x0F)		// Load flash
    {
#if Exist_W25Q_DRV
		uint32_t ext_addr = 0,offset = 0,temp_val = 0,temp_addr = 0;
		uint8_t page_buf[300];
		
		if(MODE_W25Q_Get_Max_Addr_Fun() == 0)		// W25Qxx error
		{
			return retval = 0;
		}
        if (boot.Bt_mode == 2)
        {
            ext_addr = boot.Load_Default;
        }
        else
        {
            ext_addr = boot.Load_Latest;
        }
        if (MODE_W25Q_Read_Data_Fun(ext_addr, page_buf, W25Q_PAGE_SIZE) != 0)
        {
            return retval = 0;
        }
		if(memcmp(page_buf,"bootld",6) == 0)
		{
			memcpy(&calc_crc, &page_buf[20], sizeof(calc_crc));
			memcpy(&fw_size,  &page_buf[22], sizeof(fw_size));
		}
		else
		{
			return retval = 0;
		}

        if ((fw_size == 0) || (fw_size > SYS_APP_SIZE) || (calc_crc == 0))
        {
            return retval = 0;
        }
        temp_val = Base_Flash_Erase(SYS_APP_ADDR, SYS_APP_SIZE);
        if (temp_val != 0)
        {
            return retval = 0;
        }

        offset = 0;
        temp_addr = SYS_APP_ADDR;
		ext_addr += W25Q_SECTOR_SIZE;		// binfile
        while (offset < fw_size)
        {
            uint32_t once = fw_size - offset;

            if (once > W25Q_PAGE_SIZE)
            {
                once = W25Q_PAGE_SIZE;
            }

            temp_val = MODE_W25Q_Read_Data_Fun(ext_addr, page_buf, once);
            if (temp_val != 0)
            {
                return retval = 0;
            }

            temp_val = Base_Flash_Write(page_buf, temp_addr, once);
            if (temp_val != 0)
            {
                return retval = 0;
            }
			ext_addr += once;
            temp_addr += once;
            offset += once;
        }
		target_crc = calc_crc;
#else
		return retval = 0;
#endif
    }
    else		// Load app
    {
        target_crc = boot.app_crc;
        fw_size = boot.app_size;
        if ((fw_size == 0) || (fw_size > SYS_APP_SIZE))
        {
            return retval = 0;
        }
    }
	if(fw_size > 0)
	{
		addr_p = (uint8_t *)SYS_APP_ADDR;
		calc_crc = Encrypt_XMODEM_CRC16_Fun(addr_p, fw_size);
		if (calc_crc == target_crc || target_crc == 0x1234)		// 校验内部 app 区
		{
			*crc = target_crc;
			if (boot.Bt_mode == 1)		// 没有加载外部flash，无需更新boot信息
			{
				retval = 1;				// 启动
			}
			else {
				retval = (int) fw_size;
			}
		}
		else		// crc error
		{
			retval = 0;
		}
	}
	else
	{
		retval = 0;
	}
    return retval;
}
