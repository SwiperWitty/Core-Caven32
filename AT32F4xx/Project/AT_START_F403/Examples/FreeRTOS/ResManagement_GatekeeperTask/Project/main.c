/**
  ******************************************************************************
  * File   : FreeRTOS/ResManagement_GatekeeperTask/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
#include "includes.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_ResManagement_GatekeeperTask
  * @{
  */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
static void AppTaskCreate (void);
static void AppObjCreate(void);
//void App_Printf(const char *format, ...);

static void vTaskLED(void *pvParameters);
static void vTaskMsgProcess(void *pvParameters);
static void vTaskCtrolProcess(void *pvParameters);
static void TMR_CallBack1(void);
static void TMR_CallBack2(void);
static void prvPrintGatekeeperTask(void *pvParameters);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Declare variables that are used to hold the handle of task */
static TaskHandle_t xHandleTaskLED          = NULL;
static TaskHandle_t xHandleTaskMsgProcess   = NULL;
static TaskHandle_t xHandleTaskGatekeeper = NULL;
static QueueHandle_t xHandleQueuePrint      = NULL;//used for a task transmit information to the gatekeeper task

/* Define the strings that the tasks and interrupt will print out via the gatekeeper */
static char *pcStringToPrint[] = 
{
    "Task 1 ***************************************\r\n",
    "Task 2 ---------------------------------------\r\n",
    "Message printed from the tick hook interrupt##\r\n"
};

/**
 * @brief   Main program
 * @param   None
 * @retval  None
 */
int main(void)
{
	__set_PRIMASK(1); //disable global interrupts 
	
	/* Perform any hardware setup necessary */
	bsp_Init(); 

    /* setup a timer for gather task statistics */
	vTimerInitForRunTimeState();
	
	/* Application tasks created */
	AppTaskCreate();

    /* Application communication mechanism created */
	AppObjCreate();
	
    /* Start the created tasks running */
    vTaskStartScheduler();

    /* If all is well then main() will never reach here as the scheduler will
     * now be running the tasks. If main() does reach here then it is likely
     * that there was insufficient heap memory available for the idle task to be 
     * created
     */
	while(1);
    //return 0;
}

/**
 * @brief   LED task
 * @param   pvParameters: passed to the task
 * @retval  None
 */

static void vTaskLED(void *pvParameters)
{
    while(1)
    {
		bsp_LedToggle(2);//LED2
		vTaskDelay(pdMS_TO_TICKS(1000));//delay 1s
	}
}

/**
 * @brief   obtain task run-time statistics 
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskMsgProcess(void *pvParameters)
{
    uint8_t ucCommandValue=0xFF;
    uint8_t pcWriteBuffer[500];
    char * pCharacter; 
    
    //App_Printf("\r\n1. Print task run-time statistics\r\n");    
    //App_Printf("2. Enable Timer CC1 interrupt\r\n");  
    //App_Printf("3. Enable Timer CC2 interrupt\r\n");  
    while(1)
    {
        comGetChar(COM1,&ucCommandValue);
        switch(ucCommandValue)
        {
            case '1':
                pCharacter = "==================================================\r\n";
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);
                pCharacter = "Name                  State  Priority  Stack   Num\r\n";
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);

                vTaskList((char*)pcWriteBuffer);
                pCharacter = (char *)&(pcWriteBuffer[0]);
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);

                pCharacter = "\r\n";
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);

                pCharacter = "Task                  Abs Time         %Time      \r\n";
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);

                pCharacter = "***************************************************\r\n";
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);

                vTaskGetRunTimeStats((char*)pcWriteBuffer);
                pCharacter = (char *)&(pcWriteBuffer[0]);
                xQueueSendToBack(xHandleQueuePrint, &pCharacter, 0);
                break;
            case '2':
                bsp_StartHardTimer(1, 50000, (void*)TMR_CallBack1);//after 50ms to execute ISR, and then disable CC1 interrupt
                break;
            case '3':
                bsp_StartHardTimer(2, 50000, (void*)TMR_CallBack2);//after 50ms to execute ISR, and then disable CC2 interrupt
                break;
            default:
                break;
        }

        if(ucCommandValue =='1' || ucCommandValue =='2' || ucCommandValue =='3')
        {
            //App_Printf("1. Print task run-time statistics\r\n");
            //App_Printf("2. Enable Timer CC1 interrupt\r\n");  
            //App_Printf("3. Enable Timer CC2 interrupt\r\n"); 
            comClearRxFifo(COM1);
            ucCommandValue = 0xFF;
        }
           
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief   Send a string to the queue
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskCtrolProcess(void *pvParameters)
{
    int iIndexToString;

    /* Two instances of this task are created. The task parameter is used to
     * pass an index into an array of strings into the task.
     */

    iIndexToString = (int)pvParameters;

    while(1)
    {
        /* Print out the string, not directly, but instead by passing a pointer
         * to the string to the gatekeeper task via a queue. 
         */
        xQueueSendToBack(xHandleQueuePrint, &(pcStringToPrint[iIndexToString]), 0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

/**
 * @brief   Create tasks
 * @param   None
 * @retval  None
 */
static void AppTaskCreate (void)
{
    BaseType_t xReturn;
    
    xReturn = xTaskCreate(  vTaskLED,   	    // function of task
                            "vTaskLED",         // task name. used for debugging aid only*/
                            512,               	// stack size. specify the number of words(32bits) */
                            NULL,              	// task parameter*/
                            2,                 	// task priority. Note: the smaller number, the lower priority*/
                            &xHandleTaskLED );  // task handle. used to reference the task in API calls, for example, change the task priority or delete the task*/
	if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
	xReturn = xTaskCreate(  vTaskMsgProcess,     		
                            "vTaskMsgProcess",   		
                            512,             		
                            NULL,           		
                            3,               		
                            &xHandleTaskMsgProcess );  
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
	
	xReturn = xTaskCreate(  vTaskCtrolProcess,     		
                            "vTaskCtrolProcess1",   		
                            512,            		
                            (void*)0,           		
                            4,              		
                            NULL ); 
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}

	xReturn = xTaskCreate(  vTaskCtrolProcess,     		
                            "vTaskCtrolProcess2",   		
                            512,            		
                            (void*)1,           		
                            5,              		
                            NULL ); 
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}

	xReturn = xTaskCreate(  prvPrintGatekeeperTask,     		
                            "prvPrintGatekeeperTask",   		
                            512,            		
                            NULL,           		
                            5,              		
                            &xHandleTaskGatekeeper ); 
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
	
}

/**
 * @brief   Create queue
 * @param   None
 * @retval  None
 */
static void AppObjCreate(void)
{
    xHandleQueuePrint = xQueueCreate(20, sizeof(char*));

    if(xHandleQueuePrint == NULL)
    {
        printf("There is insufficient heap memory\r\n");
    }
}

/**
 * @brief   Receive from the queue and print string
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void prvPrintGatekeeperTask(void * pvParameters)
{
    char *pcMessageToPrint;

    /* This is the only task that is allowed to write to standard out.
     * Any other task wanting to write a string to the output does not
     * access standard out directly, but instead sends the string to this
     * task. As only this task accesses standart out there are no mutual 
     * exclusion or serialization issues to consider within the implementation
     * of the task itself
     */
    while(1)
    {
        /* Wait for a message to arrive. An indefinite block time is specified
         * so there is no need to check the return value - the function will only
         * return when a message has been successfully received
         */
        xQueueReceive(xHandleQueuePrint, &pcMessageToPrint, portMAX_DELAY);

        /* Output the received string */
        printf("%s\r\n", pcMessageToPrint);

        /* Loop back to wait for the next message */
    }   
}

#if 0
/**
 * @brief   Print function
 * @param   format: things to print
 * @retval  None
 */
void App_Printf(const char *format, ...)
{
    char buf_str[200+1];//max number contain character is 200
    va_list v_args;

    va_start(v_args, format);
    
    (void)vsnprintf((char*)&buf_str[0],
                    (size_t)sizeof(buf_str),
                    (char const*)format,
                                v_args);
    va_end(v_args);

    xSemaphoreTake(xHandleMutexPrint, portMAX_DELAY);//block indefinitely until mutex be obtained
    printf("%s", buf_str);
    xSemaphoreGive(xHandleMutexPrint);//the mutex must be given back
}
#endif

/**
 * @brief   Toggle led3
 * @param   None
 * @retval  None
 */
static void TMR_CallBack1(void)
{
    bsp_LedToggle(3);
}

/**
 * @brief   Toggle led4
 * @param   None
 * @retval  None
 */
static void TMR_CallBack2(void)
{
    bsp_LedToggle(4);
}

#if ( configUSE_IDLE_HOOK == 1 )
/* Call the user defined function from within the idle task.  This
 * allows the application designer to add background functionality
 * without the overhead of a separate task.
 *  
 * NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES,
 * CALL A FUNCTION THAT MIGHT BLOCK. THE FUNCTION NAME IS FIXED*/

void vApplicationIdleHook( void )
{
    while(1);
}

#endif /* configUSE_IDLE_HOOK */

#if ( configUSE_TICK_HOOK == 1 )
/* The tick hook gets called at regular intervals, even if the
 * scheduler is locked. 
 * The tick hook function execute within the context of the tick interrupt,
 * and so must be kept very short, must use only a moderate amount of stack
 * space, and must not call any FreeRTOS API functions that do not end with
 * 'FromISR()'
 * The scheduler will always execute immediately after the tick hook function
 * so interrupt safe FreeRTOS API functions called from the tick hook do not
 * need to use their pxHigherPriorityTaskWoken parameter, and the parameter
 * can be set to NULL
 *
 * NOTE: THE FUNCTION NAME IS FIXED */


void vApplicationTickHook( void )
{
    static int iCount = 0;

    /*Print out a message every 200 ticks. The message is not written out
     * directly, but sent to the gatekeeper task
     */

    iCount++;
    if(iCount >= 200)
    {
        /* As xQueueSendToFrontFromISR() is being called from the tick hook, 
         * it is not necessary to use the xHigherPriorityTaskWoken parameter
         * and the parameter is set to NULL
         */
        xQueueSendToFrontFromISR( xHandleQueuePrint,
                                  &(pcStringToPrint[2]),
                                  NULL);
        /* Reset the count ready to print out the string again in 200 tick time */
        iCount = 0;
    }

}

#endif /* configUSE_TICK_HOOK */

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
/* NOTE: THE FUNCTION NAME IS FIXED */

void vApplicationMallocFailedHook(void)
{
    while(1);
}

#endif /* configUSE_MALLOC_FAILED_HOOK */

#if (  configCHECK_FOR_STACK_OVERFLOW > 0 )
/* The stack overflow hook function.
 * xTask: handle of the task that has exceeded its stack space
 * pcTaskName: name of the task that has exceeded its stack sapce
 * NOTE: stack overflow checking can only be used on architectures
 *       that have a linear memory map
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("task: %s stack is overFlow\r\n",pcTaskName);
}
#endif

#if( configUSE_DAEMON_TASK_STARTUP_HOOK == 1 )
/* Allow the application writer to execute some code in the context of
 * this task at the point the task starts executing.  This is useful if the
 * application includes initialisation code that would benefit from
 * executing after the scheduler has been started. */

void vApplicationDaemonTaskStartupHook( void )
{
}

#endif /* configUSE_DAEMON_TASK_STARTUP_HOOK */
/**
  * @}
  */ 

/**
  * @}
  */
 
