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


ROSA_taskHandle_t * DELAYQUEUE;

uint64_t systemTick;


/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
__attribute__((__interrupt__))
void timerISR(void)
{
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;
	systemTick++;

	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	
	bool interruptTask = false;
	
	while (DELAYQUEUE->delay <= systemTick)
	{
		ROSA_taskHandle_t * tmptsk = DELAYQUEUE;
		removeDelayQueue(&DELAYQUEUE);
		rqi(&tmptsk);
		int priority = rqsearch();
		PREEMPTASK = PA[priority];
		interruptTask = true;
	}
	if (interruptTask)
	{
		ROSA_yieldFromISR();
	}

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
	PREEMPTASK = PA[priority];
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
	if (DELAYQUEUE == NULL) {
		DELAYQUEUE = *pth;
		DELAYQUEUE->nexttcb = NULL;
		return 0;
	}
	else {
		ROSA_taskHandle_t * next = DELAYQUEUE;
		ROSA_taskHandle_t * prev;
		while (next->delay > (*pth)->delay)
		{
			prev = next;
			next = next->nexttcb;
		}
		while (next->priority >= (*pth)->priority && next->delay == (*pth)->delay)
		{
			prev = next;
			next = next->nexttcb;
		}
		(*pth)->nexttcb = next;
		prev->nexttcb = *pth;
		return 0;
	}
}

/************************************************************************/
/* removeDelayQueue()													*/
/*																		*/
/* Removes the given task from the delay queue							*/
/************************************************************************/
int removeDelayQueue(ROSA_taskHandle_t ** pth)
{
	if (*pth == DELAYQUEUE)
	{
		DELAYQUEUE = NULL;
		return 0;
	}
	ROSA_taskHandle_t * pt = DELAYQUEUE;
	while (pt->nexttcb != *pth)
	{
		pt = pt->nexttcb;
	}
	pt->nexttcb = (*pth)->nexttcb;
	return 0;
}