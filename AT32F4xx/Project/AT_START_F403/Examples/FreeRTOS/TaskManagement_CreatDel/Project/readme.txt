
/**
  @page FreeRTOS_TaskManagement_CreatDel example
  
  @verbatim
  * File   : FreeRTOS/TaskManagement_CreatDel/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_TaskManagement_CreatDel example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_MUTEXES  									1
  	#define INCLUDE_vTaskSuspend 								1 to support portMAX_DELAY
  	#define INCLUDE_vTaskDelete				    			1
  	#define INCLUDE_xTaskResumeFromISR          1
  	
    This example describes how to creat tasks, delete tasks, suspend tasks 
    and resume tasks. command read from USART1

 */