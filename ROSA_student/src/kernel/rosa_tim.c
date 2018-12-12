/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

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
/* Tab size: 4 */

#include <stdint.h>
#include <stdbool.h>
#include "rosa_config.h"
#include "drivers/delay.h"
#include "kernel/rosa_int.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "drivers/led.h"


ROSA_taskHandle_t * DELAYQUEUE;

uint64_t systemTick;

//Assisting functions for handling the ready queue
extern int rqsearch(void);
extern int rqi(ROSA_taskHandle_t ** pth);
extern int rqe(ROSA_taskHandle_t ** pth);



/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
__attribute__((__interrupt__))
void timerISR(void)
{
	interruptDisable();
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;
	ROSA_taskHandle_t * tmptsk;
	bool interruptTask;
	int priority;
	
	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if (sr & AVR32_TC_CPCS_MASK)
	{
		systemTick++;
		interruptTask = false;
		
		while (DELAYQUEUE != NULL && DELAYQUEUE->delay <= systemTick)
		{
			tmptsk = DELAYQUEUE;
			removeDelayQueue(&DELAYQUEUE);
			tmptsk->delay = 0;
			rqi(&tmptsk);
			interruptTask = true;
		}
		if (interruptTask)
		{
			priority = rqsearch();
			if (EXECTASK->priority < tmptsk->priority)
			{
				PREEMPTASK = PA[priority]->nexttcb;
				interruptEnable();
				ROSA_yieldFromISR();
			}
		}
	}
	//timerClearInterrupt(); //Disabled until we know what it actually does
	interruptEnable();
}

/************************************************************************/
/* ROSA_getTickCount()													*/
/*																		*/
/* Returns the current number of system ticks, relative to start		*/
/* of the system.                                                       */
/************************************************************************/
uint64_t ROSA_getTickCount()
{
	return systemTick;
}

/************************************************************************/
/* ROSA_delay()															*/
/*																		*/
/* Suspends the calling task for the given number of ticks				*/
/************************************************************************/
int16_t ROSA_delay(uint64_t ticks)
{
	rqe(&EXECTASK);
	insertDelayQueue(&EXECTASK, ROSA_getTickCount() + ticks);
	int priority = rqsearch();
	if (priority >= 0)
	{
		PREEMPTASK = PA[priority];
	} else {
		return -1;
	}
	ROSA_yield();
	return 0;
}


/***********************************************************
 * timerPeriodSet
 *
 * Comment:
 * 	Set the timer period to 'ms' milliseconds.
 *
 **********************************************************/
int timerPeriodSet(unsigned int ms)
{
	int rc, prescale;
	int f[] = { 2, 8, 32, 128 };
	//FOSC0 / factor_prescale * time[s];
	prescale = AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5;
	rc = FOSC0 / f[prescale - 1] * ms / 1000;
	timerPrescaleSet(prescale);
	timerRCSet(rc);
	return rc * prescale / FOSC0;
}

/************************************************************************/
/* insertDelayQueue()													*/
/*																		*/
/* Inserts the given task into the delay queue, before any tasks with	*/
/* a later deadline or lower priority									*/
/************************************************************************/
int insertDelayQueue(ROSA_taskHandle_t ** pth, uint64_t deadline)
{
	(*pth)->delay = deadline;
	
	if (DELAYQUEUE == NULL) {
		DELAYQUEUE = *pth;
		DELAYQUEUE->nexttcb = NULL;
		return 0;
	}
	
	ROSA_taskHandle_t * next = DELAYQUEUE;
	ROSA_taskHandle_t * prev;

	// While the next task in the list has an earlier deadline or higher priority and an equal deadline, move down the list
	while (next->delay <= (*pth)->delay || (next->priority >= (*pth)->priority && next->delay == (*pth)->delay))
	{
		prev = next;
		next = next->nexttcb;
		
		// Reach the end of the list
		if (next == NULL) {
			prev->nexttcb = *pth;
			(*pth)->nexttcb = NULL;
			return 0;
		}
	}
	
	(*pth)->nexttcb = next;
	prev->nexttcb = *pth;
	return 0;
}

/************************************************************************/
/* removeDelayQueue()													*/
/*																		*/
/* Removes the given task from the delay queue							*/
/************************************************************************/
int removeDelayQueue(ROSA_taskHandle_t ** pth)
{
	// If there are no tasks in the delay queue, return error code -1
	if (DELAYQUEUE == NULL)
	{
		return -1;
	}
	// If there is only one task in the status queue and this is pth, remove it from the queue
	if (DELAYQUEUE->id == (*pth)->id)
	{
		if (DELAYQUEUE->nexttcb == NULL)
		{
			DELAYQUEUE = NULL; // Task was the only one in the list
		} else {
			DELAYQUEUE = (*pth)->nexttcb;
		}
		return 0;
	}
	// Else, find the task before pth and point it to the task after pth, removing it
	ROSA_taskHandle_t * next = DELAYQUEUE;
	ROSA_taskHandle_t * prev;
	while (next->id != (*pth)->id)
	{
		prev = next;
		next = next->nexttcb;
		if(next == NULL)
		{
			return -1; //Task is not in the list, so return error code -1
		}
	}
	prev->nexttcb = next->nexttcb;
	next->nexttcb = NULL;
	return 0;
}
