
/**
  @page FreeRTOS_ResManagement_GatekeeperTask example
  
  @verbatim
  * File   : FreeRTOS/ResManagement_GatekeeperTask/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the FreeRTOS_ResManagement_GatekeeperTask example.
  ******************************************************************************
   @endverbatim

  @brief
  	#define configUSE_TICK_HOOK 1
  	
  	This example demonstrates using gatekeeper task to manage share resource. This time, a gatekeeper task is used to 
	manage access to standard out. When a task wants to write a message to standard out, it does not call a print 
	function directly but, instead, sends the message to the gatekeeper. The gatekeeper task uses a FreeRTOS queue to 
	serialize access to standard out. The internal implementation of the task does not have to consider mutual exclusion 
	because it is the only task permitted to access standard out directly.

    Note: Interrupts can send to queues, so interrupt service routines can also safely use the services of the 
	gatekeeper to write messages to the terminal

 */