
/**
  @page FreeRTOS_EvtGroup_BlockOnMultiEventsISR example
  
  @verbatim
  * File   : FreeRTOS/EvtGroup_BlockOnMultiEventsISR/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_EvtGroup_BlockOnMultiEventsISR example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_MUTEXES  		1
  	#define INCLUDE_vTaskSuspend 	1 
  	#define INCLUDE_xEventGroupSetBitsFromISR 	1
  	#define INCLUDE_xTimerPendFunctionCall 		1
  	#define configUSE_TIMERS					1
  	
    This example describes how to use the event group. Input '2' to set bit 0 of 
    event group, input '3' to set bit 1 of event group. Print information only 
    when both bit 0 and 1 of event group are 1.


 */