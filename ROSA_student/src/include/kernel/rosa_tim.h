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

/**
 * @file rosa_tim.h
 * @brief Header file containing declarations for ROSA timer functions.
 *
 * Header file containing declarations of ROSA timer functions.
 */

/* Tab size: 4 */

#ifndef _ROSA_TIMER_H_
#define _ROSA_TIMER_H_

#include <avr32/io.h>
#include <stdint.h>
#include "kernel/rosa_ker.h"

/***********************************************************
 * Kernel timer functions
 ***********************************************************/

/** @defgroup rosa_kernel ROSA kernel.
	@brief This module contains kernel functions of ROSA.
  */
  ///@{

/**
	Initialize the first timer.
	@param Number of milliseconds per system tick to be used
*/
extern void timerInit(unsigned int);

/**
	Reset the first timer.
*/
extern void timerReset(void);

/**
	Start the first timer.
*/
extern void timerStart(void);

/**
	Stop the first timer.
*/
extern void timerStop(void);

/**
	The ISR for the first timer.
*/
void timerISR(void);

/**
	Set the flag for the interrupt of the first timer to false.
*/
extern void timerClearInterrupt(void);

/**
	Set the period of the first timer.
	@param ms Period in milliseconds.
	@return 
*/
int timerPeriodSet(unsigned int ms);

extern void timerPrescaleSet(int);
extern void timerRCSet(int);

/**
	Retrieve the current number of system ticks.
	@return The current number of system ticks.
*/
uint64_t ROSA_getTickCount();


extern int timerPrescale;
extern int timerRC;

/**
	The number of ticks the system is currently on.
 */
extern uint64_t systemTick;
///@}

#endif /* _ROSA_TIMER_H_ */
