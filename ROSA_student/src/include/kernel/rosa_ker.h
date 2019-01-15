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

/**
 * @file rosa_ker.h
 * @brief Header file containing declarations for ROSA kernel's functions.
 *
 * Header file containing declarations of ROSA kernel's functions.
 */

/* Tab size: 4 */

#ifndef _ROSA_KER_H_
#define _ROSA_KER_H_

#include "rosa_def.h"

extern tcb * TCBLIST;
extern tcb * EXECTASK;
extern tcb * PREEMPTASK;
extern tcb * PA[MAXNPRIO];
extern void ROSA_contextInit(tcb * tcbTask);
extern void ROSA_yield(void);

/** @defgroup rosa_kernel ROSA kernel.
	@brief This module contains kernel functions of ROSA.
  */

	/** @defgroup rosa_kernel_sm Scheduler management.
		@ingroup rosa_kernel
		@brief Scheduler initialization and start.
	  */
	///@{
/** @def ROSA_INITIALSR
 *	@brief Define the initial value of the status register.
 */
#define ROSA_INITIALSR 0x1c0000

/**
	Initialize the ROSA system.
*/
void ROSA_init(void);

/**
	Start running the ROSA kernel.
*/
extern void ROSA_start(void);

/**
	Start running the ROSA kernel for FPPS.
*/
extern void ROSA_startScheduler(void);
	///@}

	/** @defgroup rosa_kernel_tm Task management.
		@ingroup rosa_kernel
		@brief Task creation and deletion.
	  */
	///@{
/**
	Create the TCB with correct values.
	@param tcbTask Pointer to the tcb.
	@param tcbName Name of the task.
	@param tcbFunction TCB function.
	@param tcbStack Pointer to the tcb stack.
	@param tcbStackSize Stack size of the tcb stack.
*/
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize);

/**
	Create the FPPS task with correct values.
	@param th Pointer to the task.
	@param id Id of the task.
	@param tcbFunction Task function.
	@param stackSize Stack size of the tcb stack.
	@param priority Priority of the task.
	@return Status code.
*/
int16_t ROSA_taskCreate(ROSA_taskHandle_t * th, char * id, void* taskFunction, uint32_t stackSize, uint8_t priority);

/**
	Create the FPPS task with correct values.
	@param th Pointer to the task.
	@return Status code.
*/
int16_t ROSA_taskDelete(ROSA_taskHandle_t th);

/**
	Install a new task TCB into ROSA.
	@param task Pointer to the tcb.
*/
extern void ROSA_tcbInstall(tcb *task);
	///@}
	
/** @defgroup rosa_kernel_dm Delays.
		@ingroup rosa_kernel
		@brief Task delays.
	  */
	///@{	

/**
	Delay the calling task for the given number of ticks.
	@param ticks Number of ticks to delay the task for.
	@return Status code.
*/
int16_t ROSA_delay(uint64_t ticks);

/**
	Delay the calling task until the system reaches *LastWakeTime + ticks.
	@param LastWakeTime Pointer to the time the calling task was last started.
	@param ticks Number of ticks relative to LastWakeTime.
	@return Status code.
*/
int16_t ROSA_delayUntil(uint64_t* LastWakeTime, uint64_t ticks);

/**
	Delay the calling task until the system reaches the given number of ticks.
	@param ticks Number of ticks of the system to delay the task until.
	@return Status code.
*/
int16_t ROSA_delayAbsolute(uint64_t ticks);
///@}

#endif /* _ROSA_KER_H_ */
