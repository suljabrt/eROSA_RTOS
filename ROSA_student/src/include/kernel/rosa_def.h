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
 * @file rosa_def.h
 * @author 
 * @date 05.12.2018
 * @brief Header file containing basic definitions for ROSA.
 *
 */

/* Tab size: 4 */
#ifndef rosa_def_H_
#define rosa_def_H_

#include "rosa_config.h"
#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

/** @struct tcb_record_t
    @var tcb_record_t::nexttcb 
		A pointer to the next TCB.
    @var tcb_record_t::id[NAMESIZE] 
		The task id/name.
	@var tcb_record_t::staddr 
		Start address.
	@var tcb_record_t::dataarea 
		The stack data area.
	@var tcb_record_t::datasize 
		The stack size.
	@var tcb_record_t::saveusp 
		The current stack position.
	@var tcb_record_t::SAVER0 
		Temporary work register.
	@var tcb_record_t::SAVER1 
		Temporary work register.
	@var tcb_record_t::savesr 
		The current status register.
	@var tcb_record_t::retaddr 
		The return address.
	@var tcb_record_t::savereg 
		The CPU registers.
	@var tcb_record_t::priority 
		The priority of a task.
	@var tcb_record_t::delay 
		Time until which a task is delayed.
	@var tcb_record_t::counter 
		Number of semaphores currently locked by this task.
*/
typedef struct tcb_record_t {
	struct tcb_record_t * nexttcb;
	char id[NAMESIZE];		//The task id/name
	void (*staddr) (void);	//Start address
	int *dataarea;			//The stack data area
	int datasize;			//The stack size
	int *saveusp;			//The current stack position
	int SAVER0;				//Temporary work register
	int SAVER1;				//Temporary work register
	int savesr;				//The current status register
	int retaddr;			//The return address
	int savereg[15];		//The CPU registers
	uint8_t priority;		//The priority of a task
	uint64_t delay;			//Time until which a task is delayed
	uint8_t counter;		//Number of semaphores currently locked by this task
	uint8_t originalPriority; //First priority assigned to task when creating it
} tcb;

/** @typedef tcb ROSA_taskHandle_t 
    @brief 	This struct contains all the necessary information
	to do preemption.
	@todo ROSA_taskHandle_t should be redefined as pointer to the tcb.
*/
//typedef tcb ROSA_taskHandle_t;
typedef tcb** ROSA_taskHandle_t; 
#endif /* rosa_def_H_ */


