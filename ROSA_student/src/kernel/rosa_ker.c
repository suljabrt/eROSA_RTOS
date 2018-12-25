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

#define ROSA_TM_ACTION(queue, task, action)	\
		{									\
			TCBLIST = queue;				\
			ROSA_tcb##action(task);			\
			queue = TCBLIST;				\
		}										

/** @def IDLE_STACK_SIZE
	@brief Idle task's stack size
*/
#define SYS_TASK_STACK_SIZE 32

#define MEM_CHECK(expr) if (expr == NULL) return -1;

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
tcb DELHANDL_TCB;

/** @var tcb * IDLETASK 
    @brief Global variable that contains the idle task's TCB,
	which preempts the current running task.
*/
tcb * IDLETASK;
tcb * DELHANDL;

/** @var static int sysTask_stack[SYS_TASK_STACK_SIZE]
	@brief System tasks' stack.
*/
static int idle_stack[SYS_TASK_STACK_SIZE];
static int dlay_stack[SYS_TASK_STACK_SIZE];

/** @var ROSA_taskHandle_t * PA[MAXNPRIO] 
    @brief Global array of pointers to the priority queue of the running tasks.
*/
tcb * PA[MAXNPRIO];
tcb * DQ;

/** @fn void idle(void)
	@brief Idle task body.
*/
void idle(void)
{
	while(1)
	{
		ledToggle(LED7_GPIO);
		//usartWriteLine(USART, "idle\n");
	}
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
	/* Is this the first tcb installed? */
	if(TCBLIST == NULL)
	{
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	else
	{
		tcbTask->nexttcb = TCBLIST->nexttcb;
		TCBLIST->nexttcb = tcbTask;
		TCBLIST = tcbTask;
	}
}

void ROSA_tcbUninstall(tcb * tcbTask)
{
	while (TCBLIST->nexttcb != tcbTask)
	{
		TCBLIST = TCBLIST->nexttcb;
	}
	
	TCBLIST->nexttcb = tcbTask->nexttcb;
	tcbTask->nexttcb = NULL;
	
	if (tcbTask->delay)	
	{
		TCBLIST = TCBLIST->nexttcb;
	}
	else if (TCBLIST == tcbTask)
	{
		TCBLIST = NULL;	
	}
}

/** @fn int readyQueueSearch(void)
	@brief Search for the first non-empty highest priority queue.
	@return Pointer to the last tcb in the queue (in other words - PA[i]).
*/
tcb * readyQueueSearch(void)
{
	int i = MAXNPRIO;
	
	/* Search for the first non-empty queue. */
	while ( (PA[--i] == NULL) && (i > 0));
	
	return ((i == 0) && (PA[i] == NULL)) ? IDLETASK : PA[i];
}

void dlay()
{
	tcb * tmp;
	
	while (1)
	{	
		interruptDisable();
			
		while ((DQ) && (DQ->delay <= systemTick))
		{	
			tmp = DQ;

			ROSA_TM_ACTION(DQ, DQ, Uninstall);
		
			tmp->delay = 0;
			ROSA_TM_ACTION(PA[tmp->priority], tmp, Install);
		}

		PREEMPTASK = readyQueueSearch();
		
		interruptEnable();
		
		ROSA_yield();
	}
}

/** @fn void idleCreate(void)
	@brief Creation of the idle task.
*/
void sysTasksCreate(void)
{
	ROSA_tcbCreate(&IDLETASK_TCB, "idle", idle, idle_stack, SYS_TASK_STACK_SIZE);
	IDLETASK = &IDLETASK_TCB;
	
	ROSA_tcbCreate(&DELHANDL_TCB, "dlay", dlay, dlay_stack, SYS_TASK_STACK_SIZE);
	DELHANDL = &DELHANDL_TCB;
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
	usartWriteLine(USART, "USART initialized\r\n");

	interruptInit();
	interruptEnable();
	timerInit(1);
	timerStart();
	
	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;
	PREEMPTASK = NULL;
	DQ = NULL;
	LOCKEDSEMAPHORELIST=NULL;
	
	/* Create system's tasks (idle, delay). */
	sysTasksCreate();
	
	for (i = 0; i < MAXNPRIO; PA[i++] = NULL);
}

void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize)
{
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++)
	{
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

int16_t ROSA_taskCreate(ROSA_taskHandle_t ** pth, char * id, void* taskFunction, uint32_t stackSize, uint8_t prio)
{
	int * tcbStack;
	
	tcbStack = (int *) malloc(stackSize * sizeof(uint32_t)); 
	MEM_CHECK(tcbStack);
	
	*pth = (ROSA_taskHandle_t *) malloc(sizeof(ROSA_taskHandle_t));
	MEM_CHECK(*pth);
	
	(*pth)->priority = prio;
	(*pth)->delay = 0;
	(*pth)->counter = 0;
	(*pth)->originalPriority = prio;
	
	ROSA_tcbCreate(*pth, id, taskFunction, tcbStack, stackSize);
	ROSA_TM_ACTION(PA[(*pth)->priority], *pth, Install);
		
	if ((EXECTASK) && (EXECTASK->priority < prio))
	{
		PREEMPTASK = PA[prio];
		ROSA_yield();
	}	
	
	return 0;
}

int16_t ROSA_taskDelete(ROSA_taskHandle_t ** pth)
{
	MEM_CHECK(*pth);
			
	/* Extract task from its queue */
	if ((*pth)->delay)
	{
		ROSA_TM_ACTION(DQ, *pth, Uninstall);
	}
	else
	{
		ROSA_TM_ACTION(PA[(*pth)->priority], *pth, Uninstall);
	}
	
	/* Check for itself deletion */
	if (EXECTASK == (*pth)) 
	{
		/* Check the current queue for emptiness */
		if (PA[(*pth)->priority])
		{
			PREEMPTASK = PA[(*pth)->priority]->nexttcb;
		}
		else
		{
			PREEMPTASK = readyQueueSearch();
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

/************************************************************************/
/* ROSA_delay()															*/
/*																		*/
/* Suspends the calling task for the given number of ticks				*/
/************************************************************************/
int16_t ROSA_delay(uint64_t ticks)
{
	uint64_t dv;
	uint8_t pr;
	
	interruptDisable();
	
	dv = ROSA_getTickCount() + ticks;
	pr = EXECTASK->priority;
	
	EXECTASK->delay = dv;

	/* Extract task from its queue */
	ROSA_TM_ACTION(PA[pr], EXECTASK, Uninstall);
	
	/* Check the current queue for emptiness */	
	if (PA[pr])
	{
		PREEMPTASK = PA[pr]->nexttcb;
	}
	else
	{
		PREEMPTASK = readyQueueSearch();	
	}
	
	TCBLIST = DQ;
	
	/* Search for the right place to insert the task to the delay queue */	
	while ((DQ) && (TCBLIST->delay <= dv) && (dv <= TCBLIST->nexttcb->delay))
	{
		TCBLIST = TCBLIST->nexttcb;
	}

	ROSA_tcbInstall(EXECTASK);
	DQ = TCBLIST;
	
	usartWriteTcb(USART, DQ);
	usartWriteTcb(USART, DQ->nexttcb);
	
	interruptEnable();
	
	ROSA_yield();
	
	return 0;
}
