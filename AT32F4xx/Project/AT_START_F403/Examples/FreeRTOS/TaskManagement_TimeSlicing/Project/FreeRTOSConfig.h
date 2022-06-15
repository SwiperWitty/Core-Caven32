/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "at32f4xx.h"
#include "core_cm4.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1 //kernal will execute during each tick interrupt, which can result in a context switch occurring in the tick interrupt
#define configUSE_TIME_SLICING          1 //1:will switch between tasks of equal priority on every RTOS tick interrupt. 0:will not switch between tasks of equal priority just because a tick interrupt executed
#define configUSE_TICKLESS_IDLE         0 //1:low power tickless mode. 0:keep the tick interrupt running at all times
#define configIDLE_SHOULD_YIELD			1 

#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				( 192000000UL )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )     //unit: Hz. pdMS_TO_TICKS() can only be used when configTICK_RATE_HZ is less than or equal to 1000
#define configMAX_PRIORITIES			( 10 )                       //priority that be assigned to a task
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )  //stack allocated to the Idle task. Specified in words
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 20 * 1024 ) )    //in bytes
#define configMAX_TASK_NAME_LEN			( 20 )
#define configUSE_TRACE_FACILITY		1 //1:additional structure members and functions that assist with execution visualization and tracing being included in the build
#define configUSE_16_BIT_TICKS			0 //1:TickType_t is defined to be an unsigned 16-bit type. 0: to be an unsigned 32-bit type

#define configUSE_MUTEXES				1
#define configQUEUE_REGISTRY_SIZE		8 //maximum number of queues and semaphores that can be referenced from the queue registry at any one time
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1
#define configGENERATE_RUN_TIME_STATS	1
#define configUSE_DAEMON_TASK_STARTUP_HOOK  0 //Any application initialization code that needs the RTOS to be running can be placed in the hook function

#define configAPPLICATION_ALLOCATED_HEAP            0
#define configSUPPORT_DYNAMIC_ALLOCATION            1
#define configSUPPORT_STATIC_ALLOCATION             0 //set to 1 then RTOS objects can be created using RAM provided by the application writer.
#define configENABLE_BACKWARD_COMPATIBILITY         0
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0 //only used by FreeRTOS MPU
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS     0 //set the number of indexes in the task's control block
#define configUSE_ALTERNATIVE_API 0 //use of the alternative API is no longer recommended
#define configUSE_APPLICATION_TASK_TAG              0
#define configUSE_NEWLIB_REENTRANT  0 // Newlib is a C library intended for use on embedded systems
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_QUEUE_SETS 0 //1: enable block on multiple queues at the same time
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configUSE_TASK_NOTIFICATIONS    1//include direct to task notification functionality. Then each task consumes 8 additional bytes of RAM

#if ( configGENERATE_RUN_TIME_STATS == 1)
extern volatile uint32_t ulHighFrequencyTimerTicks;
/*
 *This macro must be provided to initialize whichever peripheral is used to 
 *generate the time base. The time base used by the run time stats must have
 *a higher resolution than the tick interrupt, otherwise the gathered statistics
 *may be too inaccurate to be truly useful. It is recommended to make the time base
 *between 10 and 20 times faster than the tick interrupt
*/
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (ulHighFrequencyTimerTicks = 0ul)
/*
 * The macro must be provided to return the current time base value. Which is the
 * the total time that the application has been running in the chosen time base units
 */
#define portGET_RUN_TIME_COUNTER_VALUE()  ulHighFrequencyTimerTicks
#endif /* configGENERATE_RUN_TIME_STATS */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		    0 //Co-routines are light weight tasks that save RAM by sharing a stack, but have limited functionality.
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions.  This example uses I2C to write to the LEDs.  As
this takes a finite time, and because a timer callback writes to an LED, the
priority of the timer task is kept to a minimum to ensure it does not disrupt
test tasks that check their own execution times. */
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		( 1 )//bigger than Idle task(0)
#define configTIMER_QUEUE_LENGTH		10 //the maximum number of unprocessed commands that the timer command queue can hold at any one time
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )// in words, the size of the stack allocated to the timer service task

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		    1
#define INCLUDE_uxTaskPriorityGet		    1
#define INCLUDE_vTaskDelete				    1
#define INCLUDE_vTaskCleanUpResources	    1 //no used
#define INCLUDE_vTaskSuspend			    1
#define INCLUDE_vTaskDelayUntil			    1
#define INCLUDE_vTaskDelay				    1
#define INCLUDE_xTaskGetCurrentTaskHandle   0
#define INCLUDE_xTaskGetHandle              0
#define INCLUDE_xTaskGetIdleTaskHandle      0
#define INCLUDE_xTaskGetSchedulerState      0
#define INCLUDE_xTaskGetStackHighWaterMark  0 //no used
#define INCLUDE_xTaskResumeFromISR          1
#define INCLUDE_eTaskGetState               0
#define INCLUDE_xTimerPendFunctionCall      0
#define INCLUDE_xEventGroupSetBitsFromISR   0 //no used
#define INCLUDE_xSemaphoreGetMutexHolder    0
#define INCLUDE_xTaskAbortDelay             0

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __NVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		5        /* 32 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0f

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	1

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. 
 * Interrupt safe FreeRTOS functions (those that end in "FromISR" must not
 * be called from an interrupt that has a priority above that set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY.  */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

