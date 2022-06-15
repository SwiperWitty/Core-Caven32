/**
  @page FreeRTOS_DevSupport_TaskStateInformation example
  
  @verbatim
  * File   : FreeRTOS/DevSupport_TaskStateInformation/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_DevSupport_TaskStateInformation example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_MUTEXES  1
  	#define INCLUDE_vTaskSuspend 1 to support portMAX_DELAY

  	Receive '1' from USART1 to print tasks run-time statistcs information to USART1
    Using mutex to make a thread safe print function

    uxTaskGetSystemState(): obtain binary run-time statistics information
    vTaskGetRunTimeStats(): obtian run-time statistics information as a human readable
                            ASCII table.

   	Configuring an Application to Collect Run-Time Statistics:
   		define them in FreeRTOSConfig.h
   		(1)configGENERATE_RUN_TIME_STATS 1 -> The scheduler will call the other macros 
   		                                      detailed in this table at the appropriate 
   		                                      times.
   		(2)portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() -> This macro must be provided to 
   		                                               initialize whichever peripheral
   		                                               is used to provide the run-time
   		                                               statistics clock.
   		(3)portGET_RUN_TIME_COUNTER_VALUE()  -> This is the total time the application
   		                                        has been running, in run-time statistics 
   		                                        clock units, since the application first booted.

	#define configUSE_TRACE_FACILITY   1 
	#define configUSE_STATS_FORMATTING_FUNCTIONS 1 are both set to 1 to use vTaskList()

	#define configGENERATE_RUN_TIME_STATS 1
	#define configUSE_STATS_FORMATTING_FUNCTIONS 1 are both set to 1 to use vTaskGetRunTimeStats()

 */