/**
  @page FreeRTOS_BinSemaphore_SynTaskWithISR example
  
  @verbatim
  * File   : FreeRTOS/BinSemaphore_SynTaskWithISR/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_BinSemaphore_SynTaskWithISR example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  	#define configUSE_MUTEXES  1
  	#define INCLUDE_vTaskSuspend 1 to support portMAX_DELAY
  	
    This example demonstrates how to synchronize the task with the interrupt using 
    binary semaphore. The binary semaphore is used to defer interrupt processing 
    to a task.

    The structure of the task is adequate only if interrupts occur at a relatively
    low frequency, or the interrupt will be missed.

 */