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

/** @defgroup rosa_kernel ROSA kernel.
	@brief This module contains kernel functions of ROSA.
  */
  ///@{

/** @fn extern void timerInit(unsigned int)
	@param Number of milliseconds per system tick to be used
	@brief Initialize the first timer.
*/
extern void timerInit(unsigned int);

/** @fn extern void timerReset(void)
	@brief Reset the first timer.
*/
extern void timerReset(void);

/** @fn extern void timerStart(void)
	@brief Start the first timer.
*/
extern void timerStart(void);

/** @fn extern void timerStop(void)
	@brief Stop the first timer.
*/
extern void timerStop(void);

/** @fn void timerISR(void)
	@brief The ISR for the first timer.
*/
void timerISR(void);

/** @fn extern void timerClearInterrupt(void)
	@brief Set the flag for the interrupt of the first timer to false.
*/
extern void timerClearInterrupt(void);

/** @fn int timerPeriodSet(unsigned int ms)
	@param ms Period in milliseconds.
	@brief Set the period of the first timer.
	@return 
*/
int timerPeriodSet(unsigned int ms);

extern void timerPrescaleSet(int);
extern void timerRCSet(int);

/** @fn uint64_t ROSA_getTickCount()
	@brief Retrieve the current number of system ticks.
	@return The current number of system ticks.
*/
uint64_t ROSA_getTickCount();


extern int timerPrescale;
extern int timerRC;

/** @def systemTick
 *	@brief The number of ticks the system is currently on.
 */
extern uint64_t systemTick;
///@}

#endif /* _ROSA_TIMER_H_ */
