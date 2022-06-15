
/**
  ******************************************************************************
  * File   : XMC/SRAM/readme.txt  
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : XMC SRAM example
  ******************************************************************************
  */					
	FSMC	              AT32F403A	PSRAM
	XMC_D0	                PD14	DQ[0]
	XMC_D1	                PD15	DQ[1]
	XMC_D2	                PD0	    DQ[2]
	XMC_D3	                PD1	    DQ[3]
	XMC_D4	                PE7	    DQ[4]
	XMC_D5	                PE8	    DQ[5]
	XMC_D6	                PE9	    DQ[6]
	XMC_D7	                PE10	DQ[7]
	XMC_D8	                PE11	DQ[8]
	XMC_D9	                PE12	DQ[9]
	XMC_D10               	PE13	DQ[10]
	XMC_D11	                PE14	DQ[11]
	XMC_D12	                PE15	DQ[12]
	XMC_D13	                PD8	    DQ[13]
	XMC_D14	                PD9 	DQ[14]
	XMC_D15	                PD10	DQ[15]
	XMC_A16          	    PD11	A[16]
	XMC_A17	                PD12	A[17]
	XMC_A18	                PD13	A[18]
	XMC_A19	                PE3	    A[19]
	XMC_A20	                PE4	    A[20]
	XMC_A21	                PE5	    A[21]
	XMC_A22           	    PE6	    A[22]
	XMC_NCE2 / XMC_NE1	    PD7	    CE#
	XMC_NADV	            PB7	    ADV#
	XMC_NOE	                PD4     OE#
	XMC_NWE	                PD5     WE#
	XMC_NBL0	            PE0	    LB#
	XMC_NBL1	            PE1	    UB#

	Note:
    This example provides a basic example of how to use the XMC firmware library and
	an associate driver to perform read/write operations on the W957D6HB PSRAM
	memory.

    LED2 ON: write and read all are OK
    LED3 ON: some errors
