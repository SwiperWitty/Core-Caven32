
/**
  @page FreeRTOS_TaskManagement_TimeSlicing example
  
  @verbatim
  * File   : FreeRTOS/TaskManagement_TimeSlicing/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_TaskManagement_TimeSlicing example.
  ******************************************************************************
   @endverbatim


  @brief
  	#define configUSE_MUTEXES  1
  	#define INCLUDE_vTaskSuspend 1 to support portMAX_DELAY

	#define configIDLE_SHOULD_YIELD 1  
		set to 0: then the Idle task will remain in the Running state for the 
		          entirety of its time slice, unless it is preempted by a 
		          higher priority task
		set to 1: then the Idle task will yield on each iteration of its loop
		          if there are other Idle priority tasks in the Ready state
	
	(1)Prioritized Pre-emptive Scheduling with Time Slicing
  	#define configUSE_PREEMPTION       1
  	#define configUSE_TIME_SLICING     1 //use time slicing to share processing time
  	                                       between tasks of equal priority

  	(2)Prioritiez Pre-emptive Scheduling without Time Slicing
  	#define configUSE_PREEMPTION       1
  	#define configUSE_TIME_SLICING	   0

  	(3)Co-operative Scheduling
  	#define configUSE_PREEMPTION	   0
  	#define configUSE_TIME_SLICING     Any value
	Note: When the co-operative scheduler is used, a context switch will only occur when 
	      the Running state task enters the Blocked state, or the Running state task 
	      explicity yields by calling taskYIELD(). Tasks are never pre-empted, so time
	      slicing cannot be used.
  	
    This example demonstrates how the scheduling algorithm time slicing works

 */