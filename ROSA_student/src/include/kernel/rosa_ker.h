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
 * @author Anton (?)
 * @date 05.12.2018
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
extern ROSA_taskHandle_t * PA[MAXNPRIO];
extern void ROSA_contextInit(tcb * tcbTask);
extern void ROSA_yield(void);

/** @defgroup rosa_kernel ROSA kernel.
	@brief This module contains kernel functions of ROSA.
  */
///@{
	/** @defgroup rosa_kernel_sm Scheduler management.
		@brief Scheduler initializaion and start.
	  */
	///@{
/** @def ROSA_INITIALSR
 *	@brief Define the initial value of the status register.
 
 	Define the initial value of the status register.	
 */
#define ROSA_INITIALSR 0x1c0000

/** @fn void ROSA_init(void)
	@brief Initialize the ROSA system.
	
	Initialize the ROSA system. 
*/
void ROSA_init(void);

/** @fn extern void ROSA_start(void)
	@brief Start running the ROSA kernel.
		
	This start running the created and installed tasks.
*/
extern void ROSA_start(void);

/** @fn extern void ROSA_startScheduler(void);
	@brief Start running the ROSA kernel for FPPS.
		
	This start running the created and installed tasks for FPPS.
*/
extern void ROSA_startScheduler(void);
	///@}

	/** @defgroup rosa_kernel_tm Task management.
		@brief Task creation and deletion.
	  */
	///@{
/** @fn void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize)
	@brief 	Create the TCB with correct values.
	@param tcbTask Pointer to the tcb.
	@param tcbName[NAMESIZE] Name of the task.
	@param tcbFunction TCB function.
	@param tcbStack Pointer to the tcb stack.
	@param tcbStackSize Stack size of the tcb stack.
	
	Create the TCB with correct values.
*/
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize);

/** @fn int16_t ROSA_taskCreate(ROSA_taskHandle_t ** th, char * id, void* taskFunction, uint32_t stackSize, uint8_t priority)
	@brief 	Create the FPPS task with correct values.
	@param th Pointer to the task.
	@param id Id of the task.
	@param tcbFunction Task function.
	@param stackSize Stack size of the tcb stack.
	@param priority Priority of the task.
	@return Status code.
	
	Create the FPPS task with correct values.
*/
int16_t ROSA_taskCreate(ROSA_taskHandle_t ** th, char * id, void* taskFunction, uint32_t stackSize, uint8_t priority);

/** @fn int16_t ROSA_taskDelete(ROSA_taskHandle_t ** th)
	@brief 	Create the FPPS task with correct values.
	@param th Pointer to the task.
	@return Status code.
		
	Delete the FPPS task.
*/
int16_t ROSA_taskDelete(ROSA_taskHandle_t ** th);

/** @fn extern void ROSA_tcbInstall(tcb *task)
	@brief Install a new task TCB into ROSA.
	@param task Pointer to the tcb.
		
	Install a new task TCB into ROSA.
*/
extern void ROSA_tcbInstall(tcb *task);
	///@}
///@}

#endif /* _ROSA_KER_H_ */
