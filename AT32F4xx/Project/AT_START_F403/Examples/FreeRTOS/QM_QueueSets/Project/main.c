/**
  ******************************************************************************
  * File   : FreeRTOS/QM_QueueSets/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

#include "includes.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_QM_QueueSets
  * @{
  */ 
  



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static void AppTaskCreate (void);
static void AppObjCreate(void);
void App_Printf(const char *format, ...);

static void vTaskLED(void *pvParameters);
static void vTaskMsgProcess(void *pvParameters);
static void vTaskSender1(void *pvParameters);
static void vTaskSender2(void *pvParameters);
static void vTaskReceiver(void *pvParameters);
static void TMR_CallBack1(void);
static void TMR_CallBack2(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Declare variables that are used to hold the handle of task */
static TaskHandle_t xHandleTaskLED          = NULL;
static TaskHandle_t xHandleTaskMsgProcess   = NULL;
static SemaphoreHandle_t xHandleMutexPrint = NULL;
static QueueSetHandle_t xHandleQueueSet     = NULL;
static QueueHandle_t xHandleQueue1          = NULL;
static QueueHandle_t xHandleQueue2          = NULL;

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
		//App_Printf("This is LED Task, lower priority than Ctrol task and longer information needed to print on standard output\r\n");
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
    
    //App_Printf("\r\n1. Print task run-time statistics\r\n");    
    //App_Printf("2. Enable Timer CC1 interrupt\r\n");  
    //App_Printf("3. Enable Timer CC2 interrupt\r\n");  
    while(1)
    {
        comGetChar(COM1,&ucCommandValue);
        switch(ucCommandValue)
        {
            case '1':
                App_Printf("==================================================\r\n");
                App_Printf("Name                  State  Priority  Stack   Num\r\n");
                vTaskList((char*)pcWriteBuffer);
                App_Printf("%s\r\n", pcWriteBuffer);

                App_Printf("\r\n");
                App_Printf("Task                  Abs Time         %%Time      \r\n");
                App_Printf("***************************************************\r\n");
                vTaskGetRunTimeStats((char*)pcWriteBuffer);
                App_Printf("%s\r\n", pcWriteBuffer);
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
 * @brief   Send data to the queue1
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskSender1(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xBlockTime = pdMS_TO_TICKS(200);
    const char * const pcMessage = "Message from vSenderTask1\r\n";
    
    while(1)
    {
        /* Block for 100ms */
        vTaskDelay(xBlockTime);
        
        /* Send this task's string to xQueue1. It is not necessary to 
         * use a block time, even through the queue can only hold one
         * item. This is because the priority of the task that reads from
         * the queue is higher than the priority of this task,as soon as
         * this task writes to the queue it will be pre-empted by the task
         * that reads from the queue, so the queue will already be empty again
         * by the time the call to xQueueSendToBack() returns. The block time is
         * set to 0
         */
        xStatus = xQueueSendToBack(xHandleQueue1, &pcMessage, 0);
        if(xStatus != pdPASS)
        {
            App_Printf("The queue was already full\r\n");
        }
    }

}

/**
 * @brief   Send data to the queue2
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskSender2(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xBlockTime = pdMS_TO_TICKS(200);
    const char * const pcMessage = "Message from vSenderTask2\r\n";
    
    while(1)
    {
        /* Block for 200ms */
        vTaskDelay(xBlockTime);
        
        /* Send the pointer to the queue */
        xStatus = xQueueSendToBack(xHandleQueue2, &pcMessage, 0);
        if(xStatus != pdPASS)
        {
            App_Printf("The queue was already full\r\n");
        }
    }
}

/**
 * @brief   Receive data from the queue1 or queue2
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskReceiver(void *pvParameters)
{
    QueueHandle_t xQueueThatContainsData;
    char *pcReceivedString;

    while(1)
    {
        /* Block on the queue set to wait for one of the queues in the
         * set to contain data. Cast the QueueSetMemberHandle_t value
         * returned from xQueueSelectFromSet() to a QueueHandle_t, as
         * it is known all the members of the set are queues(the queue
         * set does not contain any semaphores)
         */
        xQueueThatContainsData = (QueueHandle_t)xQueueSelectFromSet(xHandleQueueSet,portMAX_DELAY);

        /* An indefinite block time was used when reading from the queue set,
         * so xQueueSelectFromSet() will not have returned unless one of the 
         * queues in the set contained data, and xQueueThatContainsData cannot 
         * be NULL. Read from the queue. It is not necesary to specify a block
         * time because it is known the queue contains data. The block time is 
         * set to 0
         */

        xQueueReceive(xQueueThatContainsData, &pcReceivedString, 0);
        App_Printf("%s\r\n",pcReceivedString);
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
	
	xReturn = xTaskCreate(  vTaskSender1,
                            "vTaskSender_1",
                            512,
                            NULL,
                            2,
                            NULL);
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
    xReturn = xTaskCreate(  vTaskSender2,
                            "vTaskSender_2",
                            512,
                            NULL,
                            2,
                            NULL);
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
 
    xReturn = xTaskCreate(  vTaskReceiver,
                            "vTaskReceiver",
                            512,
                            NULL,
                            3,
                            NULL);
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
	
}

/**
 * @brief   Create semaphore and queue sets
 * @param   None
 * @retval  None
 */
static void AppObjCreate(void)
{
    xHandleMutexPrint = xSemaphoreCreateMutex();

    if(xHandleMutexPrint == NULL)
    {
        printf("There is insufficient heap memory\r\n");
    }

    xHandleQueue1 = xQueueCreate(1,sizeof(char*));
    if(xHandleQueue1 == NULL)
    {
        App_Printf("There is insufficient heap memory\r\n");
    }

    xHandleQueue2 = xQueueCreate(1,sizeof(char*));
    if(xHandleQueue2 == NULL)
    {
        App_Printf("There is insufficient heap memory\r\n");
    }

    /* Create the queue set. Two queues will be added to the set, each of
     * which can contain 1 tiem, so the maximum number of queue handles the 
     * queue set will ever have to hold at one time is 2 (2 queues multiplied
     * by 1 item per queue)
     */
    xHandleQueueSet = xQueueCreateSet(1*2);
    if(xHandleQueueSet == NULL)
    {
        App_Printf("There is insufficient heap memory\r\n");
    }

    /* Queues can only be added to a set when they are empty */
    xQueueAddToSet(xHandleQueue1, xHandleQueueSet);
    xQueueAddToSet(xHandleQueue2, xHandleQueueSet);
}

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
 * NOTE: THE FUNCTION NAME IS FIXED */

void vApplicationTickHook( void )
{
    while(1);
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
    App_Printf("task: %s stack is overFlow\r\n",pcTaskName);
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
  

