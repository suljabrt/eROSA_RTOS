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

/** @var ROSA_taskHandle_t * PA[MAXNPRIO] 
    @brief Global array of pointers to the priority queue of the running tasks.
*/
ROSA_taskHandle_t * PA[MAXNPRIO];

/** @fn int rqi(ROSA_taskHandle_t ** th)
	@brief Inserts the given task into the ready queue by its priority.
	@param th Task structure (tcb structure).
	@return A status code (1 - item has been added to the empty queue, 0 - otherwise).
*/
int rqi(ROSA_taskHandle_t ** pth)
{
	uint8_t priority;
	
	priority = (*pth)->priority;
	
	if (PA[priority] == NULL) {
		PA[priority] = *pth;
		PA[priority]->nexttcb = *pth;
		return 1;
	}
	else {
		(*pth)->nexttcb = PA[priority]->nexttcb;
		PA[priority]->nexttcb = *pth;
		PA[priority] = *pth;
		return 0;
	}
}

/** @fn int rqe(ROSA_taskHandle_t ** th)
	@brief Extracts the given task from the ready queue by its priority.
	@param th Task structure (tcb structure).
	@return A status code (1 - queue is empty after extraction, 0 - otherwise).
*/
int rqe(ROSA_taskHandle_t ** pth)
{
	ROSA_taskHandle_t * thTmp;
	
	uint8_t priority;
	
	priority = (*pth)->priority;
	thTmp = PA[priority];
	
	if ((*pth)->nexttcb == *pth) 
	{
		PA[priority] = NULL;
		return 1;
	}
	else 
	{
		while (thTmp->nexttcb != (*pth)) 
		{
			thTmp = thTmp->nexttcb;
		}
		
		if (PA[priority] == *pth) 
		{
			PA[priority] = thTmp;
		}
		
		thTmp->nexttcb = (*pth)->nexttcb;
		return 0;
	}
}

/** @fn int rqsearch(void)
	@brief Search for the first non-empty highest priority queue.
	@return A status code (reserved).
	@todo Check for empty PA.
*/
int rqsearch(void)
{
	int i = MAXNPRIO;
	
	while (PA[--i] == NULL) {
		;
	}
	
	return i;
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

int16_t ROSA_taskCreate(ROSA_taskHandle_t ** pth, char * id, void* taskFunction, uint32_t stackSize, uint8_t priority)
{
	int * tcbStack;
	
	tcbStack = (int *) calloc(stackSize, sizeof(uint32_t)); 
	
	*pth = (ROSA_taskHandle_t *) malloc(sizeof(ROSA_taskHandle_t));			
	(*pth)->priority = priority;
	(*pth)->delay = 0;
	(*pth)->counter = 0;
	(*pth)->originalPriority = priority;
	
	ROSA_tcbCreate(*pth, id, taskFunction, tcbStack, stackSize);
	
	rqi(pth);
	
	if (EXECTASK != NULL) {
		if (EXECTASK->priority < priority) {
			PREEMPTASK = PA[priority];
			ROSA_yield();
		}	
	}
	
	return 0;
}

int16_t ROSA_taskDelete(ROSA_taskHandle_t ** pth)
{	
	rqe(pth);
	uint8_t priority;
	
	priority = (*pth)->priority;
	
	if (EXECTASK == (*pth)) {
		if (PA[priority] == NULL) {
			priority = rqsearch();
			PREEMPTASK = PA[priority];
			free( (*pth)->dataarea - (*pth)->datasize);
			free(*pth);
			*pth = NULL;
			ROSA_yield();
		}		
		else {
			PREEMPTASK = EXECTASK->nexttcb;
			free( (*pth)->dataarea - (*pth)->datasize);
			free(*pth);
			*pth = NULL;
			ROSA_yield();
		}
	}
	
	free( (*pth)->dataarea - (*pth)->datasize);
	free(*pth);
	*pth = NULL;
	
	return 0;
}