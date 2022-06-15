/**
  ******************************************************************************
  * File   : COMP/COMP_Out/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of how to configure COMP to get the comparison result.
  ******************************************************************************
  * @Description
  * 
  *   This example shows how to configure COMP's blanking source, the TMR1_CH4 is
  * ANDed with the comparator output to provide the wanted comparator output. 
  * TMR1_CH4(PA11) output 3kHz and duty 25% PWM, input an external signal(100kHz)
  * to COMP non-inveting channel(PA1). When TMR1_CH4 PWM level is high, COMP output 
  * won't be change.
  *
  ******************************************************************************
  */ 
  
