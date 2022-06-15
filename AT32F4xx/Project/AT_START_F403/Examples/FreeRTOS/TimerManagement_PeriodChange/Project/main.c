/**
  ******************************************************************************
  * File   : FreeRTOS/TimerManagement_PeriodChange/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */
#include "includes.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_TimerManagement_PeriodChange
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define mainONE_SHOT_TIMER_PERIOD       pdMS_TO_TICKS(1000) //1s
#define mainAUTO_RELOAD_TIMER_PERIOD    pdMS_TO_TICKS(200)//0.2s
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
static void AppTaskCreate (void);
static void AppObjCreate(void);
void App_Printf(const char *format, ...);

static void vTaskLED(void *pvParameters);
static void vTaskMsgProcess(void *pvParameters);
static void vTaskCtrolProcess(void *pvParameters);
static void TMR_CallBack1(void);
static void TMR_CallBack2(void);
static void prvOneShotTimerCallback(TimerHandle_t xTimer);
static void prvAutoReloadTimerCallback(TimerHandle_t xTimer);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Declare variables that are used to hold the handle of task */
static TaskHandle_t xHandleTaskLED          = NULL;
static TaskHandle_t xHandleTaskMsgProcess   = NULL;
static TaskHandle_t xHandleTaskCtrolProcess = NULL;
static SemaphoreHandle_t xHandleMutexPrint  = NULL;
static TimerHandle_t xHandleAutoReloadTimer = NULL;
static TimerHandle_t xHandleOneShotTimer    = NULL;

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
 * @brief   Empty task
 * @param   pvParameters: passed to the task
 * @retval  None
 */
static void vTaskCtrolProcess(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
		//App_Printf("This is Ctrol Task\r\n");
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
                            "vTaskCtrolProcess",   		
                            512,            		
                            NULL,           		
                            4,              		
                            &xHandleTaskCtrolProcess ); 
    if(xReturn != pdPASS)
	{
	    while(1);//There is insufficient heap memory
	}
	
}

/**
 * @brief   Create semaphore and timer
 * @param   None
 * @retval  None
 */
static void AppObjCreate(void)
{
    BaseType_t xTimerAutoReloadStart;
    BaseType_t xTimerOneShotStart;
    
    xHandleMutexPrint = xSemaphoreCreateMutex();

    if(xHandleMutexPrint == NULL)
    {
        printf("There is insufficient heap memory\r\n");
    }

    xHandleOneShotTimer = xTimerCreate("OneShotTimer",//timer name
                                        mainONE_SHOT_TIMER_PERIOD,//timer period
                                        pdFALSE,//one-shot software timer
                                        0,//does not use timer ID
                                        prvOneShotTimerCallback);
    xHandleAutoReloadTimer = xTimerCreate("AutoReloadTimer",
                                            mainAUTO_RELOAD_TIMER_PERIOD,
                                            pdTRUE,
                                            0,//does not use timer ID
                                            prvAutoReloadTimerCallback);

    /* Check the software timers */
    if((xHandleOneShotTimer != NULL) && (xHandleAutoReloadTimer != NULL))
    {
        xTimerAutoReloadStart = xTimerStart(xHandleAutoReloadTimer, 0);
        xTimerOneShotStart    = xTimerStart(xHandleOneShotTimer, 0);

        /* The implementation of xTimerStart() uses the timer command queue,
         * and xTimerStart() will fail if the timer command queue gets full,
         * The timer service task does not get created until the scheduler is
         * started, so all commands sent to the command queue will stay in the 
         * queue until after the scheduler has been started */

        if((xTimerAutoReloadStart != pdPASS) || (xTimerOneShotStart != pdPASS))
        {
            App_Printf("Couldn't start any software timer\r\n");
        }
    }
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

#if ( configUSE_TIMERS  == 1)
/*
 * Software timer callback functions must not call FreeRTOS API functions
 * that will result in the calling task entering the Blocked state, as to 
 * do so will result in the daemon task entering the Blocked state.
 */
static void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    static uint32_t CallbackCount=0;

    xTimeNow = xTaskGetTickCount();//Obtain the current tick count
    
    App_Printf("One-shot timer callback executing: %d\r\n",xTimeNow);
    CallbackCount++;

    
    xTimerChangePeriod(xTimer,1000+CallbackCount*100,0);//change period and start running again
                                                        //do not enter blocked state
}

static void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;

    xTimeNow = xTaskGetTickCount();//Obtain the current tick count

    App_Printf("Auto-reload timer callback executing: %d\r\n",xTimeNow);
}

#endif

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
 


