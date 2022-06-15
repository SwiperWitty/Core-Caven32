/**
  @page FreeRTOS_QM_BlockOnReceive example
  
  @verbatim
  * File   : FreeRTOS/QM_BlockOnReceive/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_QM_BlockOnReceive example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_MUTEXES  1
  	#define INCLUDE_vTaskSuspend 1 to support portMAX_DELAY
  	
    This example demonstrates a queue being created, data being sent to the queue
    from multiple tasks, and data being received from the queue. The queue is created
    to hold data items of type int32_t. The tasks that send to the queue do not specify
    a block time, whereas the task that receives from the queue does.
    
    The priority of the tasks that send to the queue are lower than the priority of the 
    task that receives from the queue. This means the queue should never contain more than 
    one item. Because as soon as data is sent to the queue the receiving task will unblock,
    pre-empt the sending task, and remove the data--leaving the queue empty once again.

 */