#ifndef _BASE__FLASH_H
#define _BASE__FLASH_H

/*base_flash.h*/

#include "gd32f4xx.h"
#include <stdio.h>

void fmc_init(void);

uint32_t fmc_sector_get(uint32_t address);
void fmc_erase_sector(uint32_t fmc_sector);

int fmc_write_data(int address, const void * data,int size);
int fmc_read_data(int address, void * data,int size);

#endif

