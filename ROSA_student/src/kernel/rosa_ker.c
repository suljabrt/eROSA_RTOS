/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/**
 * @file rosa_ker.c
 * @author Anton (?)
 * @date 05.12.2018
 * @brief File containing definitions of ROSA kernel's functions.
 *
 * In this file we define data structure for tasks, functions to create and
 * delete tasks, global tasks to control this tasks data structure.
 * 
 */

/* Tab size: 4 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_semaphore.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/** @def IDLE_STACK_SIZE
	@brief Idle task's stack size
*/
#define IDLE_STACK_SIZE 32

/** @var tcb * TCBLIST 
    @brief Global variable that contains the list of TCB's that
	have been installed into the kernel with ROSA_tcbInstall().
*/
tcb * TCBLIST;

/** @var tcb * EXECTASK 
    @brief Global variable that contains the current running TCB.
*/
tcb * EXECTASK;

/** @var tcb * EXECTASK 
    @brief Global variable that contains the TCB,
	which preempts the current running task.
*/
tcb * PREEMPTASK;

/** @var tcb IDLETASK_TCB
	@brief Idle task's tcb
*/
tcb IDLETASK_TCB;

/** @var tcb * IDLETASK 
    @brief Global variable that contains the idle task's TCB,
	which preempts the current running task.
*/
tcb * IDLETASK;

/** @var static int idle_stack[IDLE_STACK_SIZE]
	@brief Idle task's stack.
*/
static int idle_stack[IDLE_STACK_SIZE];

/** @var ROSA_taskHandle_t * PA[MAXNPRIO] 
    @brief Global array of pointers to the priority queue of the running tasks.
*/
ROSA_taskHandle_t * PA[MAXNPRIO];

/** @fn void idle(void)
	@brief Idle task body.
*/
void idle(void)
{
	while(1)
	{
		usartWriteLine(USART, "idle\n");
	}
}

/** @fn void idleCreate(void)
	@brief Creation of the idle task.
*/
void idleCreate(void)
{
	ROSA_tcbCreate(&IDLETASK_TCB, "idle", idle, idle_stack, IDLE_STACK_SIZE);
	IDLETASK = &IDLETASK_TCB;
}

/** @fn int readyQueueInsert(ROSA_taskHandle_t ** th)
	@brief Inserts the given task into the ready queue by its priority.
	@param th Task structure (tcb structure).
	@return A status code (1 - item has been added to the empty queue, 0 - otherwise).
*/
int readyQueueInsert(ROSA_taskHandle_t * pth)
{
	uint8_t priority;
	int retval;
	
	priority = (*pth)->priority;
	
	/* Check the given queue for the emptiness */
	if (PA[priority] == NULL)
	{
		PA[priority] = *pth;
		PA[priority]->nexttcb = *pth;
		
		retval = 1;
	}
	else
	{
		(*pth)->nexttcb = PA[priority]->nexttcb;
		PA[priority]->nexttcb = *pth;
		PA[priority] = *pth;
		
		retval = 0;
	}
	
	return retval;
}

/** @fn int readyQueueExtract(ROSA_taskHandle_t ** th)
	@brief Extracts the given task from the ready queue by its priority.
	@param th Task structure (tcb structure).
	@return A status code (1 - queue is empty after extraction, 0 - otherwise).
*/
int readyQueueExtract(ROSA_taskHandle_t * pth)
{
	ROSA_taskHandle_t thTmp;
	uint8_t priority;
	int retval;
	
	priority = (*pth)->priority;
	thTmp = PA[priority];
	
	/* Check whether the deleted task is the last in the queue or not */
	if ((*pth)->nexttcb == *pth) 
	{
		/* It's enough to extract the deleted task from the queue */
		PA[priority] = NULL;
		
		retval = 1;
	}
	else 
	{
		/* Search for delete task */
		while (thTmp->nexttcb != (*pth)) 
		{
			thTmp = thTmp->nexttcb;
		}
		
		/* Check whether the PA[priority] points to the deleted task or not */
		if (PA[priority] == *pth)
		{
			/* Move PA[priority] pointer to the previous task in the queue */ 
			PA[priority] = thTmp;
		}
		
		thTmp->nexttcb = (*pth)->nexttcb;
		
		retval = 0;
	}
	
	return retval;
}

/** @fn int readyQueueSearch(void)
	@brief Search for the first non-empty highest priority queue.
	@return Pointer to the last tcb in the queue (in other words - PA[i]).
*/
tcb * readyQueueSearch(void)
{
	int i = MAXNPRIO;
	tcb * rettcb;
	
	/* Search for the first non-empty queue. */
	while ( (PA[--i] == NULL) && (i > 0))
	{
		;
	}
		
	/* Check the PA for emptiness. */
	if ((i == 0) && (PA[i] == NULL))
	{
		rettcb = IDLETASK;
	}
	else
	{
		rettcb = PA[i];
	}
	
	return rettcb;
}

void ROSA_init(void)
{
	int i = 0;
	systemTick = 0;
	
	//Do initialization of I/O drivers
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	interruptInit();
	interruptEnable();
	timerInit(1);
	timerStart();
	
	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;
	PREEMPTASK = NULL;
	LOCKEDSEMAPHORELIST=NULL;
	
	/* Create idle task. */
	idleCreate();
	
	for (i = 0; i < MAXNPRIO; i++) {
		PA[i] = NULL;
	}
	
	//Initialize the timer to 1 ms period.
	
}

void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize)
{
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;

	//Set the initial SR.
	tcbTask->savesr = ROSA_INITIALSR;

	//Initialize context.
	contextInit(tcbTask);
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(tcb * tcbTask)
{
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
}

int16_t ROSA_taskCreate(ROSA_taskHandle_t * pth, char * id, void* taskFunction, uint32_t stackSize, uint8_t priority)
{
	int * tcbStack;
	
	tcbStack = (int *) calloc(stackSize, sizeof(uint32_t)); 
	
	*pth = (ROSA_taskHandle_t *) malloc(sizeof(ROSA_taskHandle_t));			
	(*pth)->priority = priority;
	(*pth)->delay = 0;
	(*pth)->counter = 0;
	(*pth)->originalPriority = priority;
	
	ROSA_tcbCreate(*pth, id, taskFunction, tcbStack, stackSize);
	
	readyQueueInsert(pth);
	
	if (EXECTASK != NULL) {
		if (EXECTASK->priority < priority) {
			PREEMPTASK = PA[priority];
			ROSA_yield();
		}	
	}
	
	return 0;
}

int16_t ROSA_taskDelete(ROSA_taskHandle_t * pth)
{	
	int isEmpty = 0;
	
	/* In this case, we just check, that *pth != NULL, because
	 * if *pth == NULL, then this task has been already deleted.
	 */
	//ASSERT_MEM_ALLOC(*pth);
	
	/* Extract task from its queue */
	isEmpty = readyQueueExtract(pth);
	
	/* Check for itself deletion */
	if (EXECTASK == (*pth))
	{ 
		/* Check the current queue for emptiness */
		if (isEmpty)
		{
			PREEMPTASK = readyQueueSearch();
		}
		else
		{
			PREEMPTASK = (*pth)->nexttcb;
		}
	}
	
	/* Task's stack memory deallocation */
	free( (*pth)->dataarea - (*pth)->datasize );
	
	/* Task's memory deallocation */
	free(*pth);
	
	/* *pth must be NULL */
	*pth = NULL;

	if (PREEMPTASK != NULL)
	{
		ROSA_yield();
	}
		
	return 1;
}
