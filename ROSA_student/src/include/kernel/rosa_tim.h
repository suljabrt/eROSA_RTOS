/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'\\,

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

#ifndef _ROSA_TIMER_H_
#define _ROSA_TIMER_H_

#include <avr32/io.h>
#include <stdint.h>
#include "kernel/rosa_ker.h"

/***********************************************************
 * Kernel timer functions
 ***********************************************************/
extern void timerInit(unsigned int);
extern void timerReset(void);
extern void timerStart(void);
extern void timerStop(void);

/** @defgroup rosa_tim ROSA timer functionality.
	@brief This module contains timer functions of ROSA.
  */

	/** @defgroup rosa_timer_sm Scheduler management.
		@ingroup rosa_timer
		@brief Timer and delay functionality
	  */
	///@{

//The timer interrupt service routine
void timerISR(void);
extern void timerClearInterrupt(void);

//Timer period functions
int timerPeriodSet(unsigned int ms);
extern void timerPrescaleSet(int);
extern void timerRCSet(int);

//Functionality added by ROSA's extension

/** @fn int64_t ROSA_getTickCount()
	@brief 	Get the current number of system ticks, relative to the start of the system.
	@return The number of system ticks since system start or since tick	overflow.
*/
uint64_t ROSA_getTickCount();

/** @fn int64_t ROSA_delay()
	@brief 	Suspends the calling task for the given number of ticks.
	@param ticks Number of ticks to delay the task
	@return Means nothing.
*/
int16_t ROSA_delay(uint64_t ticks);

/** @fn int16_t ROSA_delayUntil(uint64_t* lastWakeTime, uint64_t ticks)
	@brief 	Suspends the calling task until the system ticks reach *lastWakeTime + ticks.
	@param lastWakeTime Pointer to the number of ticks at which the calling task last resumed execution.
	@param ticks The number of ticks for the task to be suspended, relative to *lastWakeTime.
	@return Means nothing.
*/
int16_t ROSA_delayUntil(uint64_t* lastWakeTime, uint64_t ticks);

/** @fn int16_t ROSA_delayAbsolute(uint64_t ticks)
	@brief 	Suspends the calling task until the given number of ticks which is relative to the start of the system.
	@param ticks The number of ticks for the task to be suspended, relative to *lastWakeTime.
	@return Means nothing.
*/
int16_t ROSA_delayAbsolute(uint64_t ticks);

//Timer period variables
extern int timerPrescale;
extern int timerRC;

//The list storing all delayed tasks

/** @var ROSA_taskHandle_t * DELAYQUEUE 
    @brief Pointer to the first task in the queue of delayed tasks.
*/
extern ROSA_taskHandle_t DELAYQUEUE;

/** @var uint64_t systemTick
    @brief Global variable with the current number of system ticks.
*/
extern uint64_t systemTick;

///@}

#endif /* _ROSA_TIMER_H_ */
