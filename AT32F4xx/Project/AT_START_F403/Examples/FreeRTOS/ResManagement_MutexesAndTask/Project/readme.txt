
/**
  @page FreeRTOS_ResManagement_MutexesAndTask example
  
  @verbatim
  * File   : FreeRTOS/ResManagement_MutexesAndTask/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_ResManagement_MutexesAndTask example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_MUTEXES  1
  	#define INCLUDE_vTaskSuspend 1 to support portMAX_DELAY
  	
    This example demonstrates using standard mutex between different tasks that have the same priority.

    Note: If two or more tasks used the same mutex in a tight loop, then processing time would be wasted by rapidly 
	switching between tha tasks. So care must be taken to ensure the tasks receive an approximately equal amount of 
	processing time.
 */