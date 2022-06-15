/**
  ******************************************************************************
  * File   : XMC/SRAM/SRAM_DataMemory/readme.txt  
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : XMC SRAM example
  ******************************************************************************
  */

	Note:
    This example shows how to use the external SRAM as program data memory and internal SRAM for Stack.
    This example doesn't use the default library startup files. 
	
	
    - in Project->Options for Target window, select 'RAM1'and enter 0x68000000 
      as start address with size of 0x100000 (IRAM1 must be un-checked)
    - uncomment "#define DATA_IN_ExtSRAM " in the "system_at32f4xx.c" file 
	
    Uncomment #define DATA_IN_ExtSRAM in system_at32f403.c to initialize the XMC in the starting stage
