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

#include "kernel/rosa_scheduler.h"
#include "rosa_config.h"

/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void scheduler(void)
{
	if (OLD_API)
	{
		EXECTASK = EXECTASK->nexttcb;
	}
	else
	{
		if (PREEMPTASK) {
			EXECTASK = PREEMPTASK;
			PREEMPTASK = NULL;
		}
		else {
			PA[EXECTASK->priority] = EXECTASK;
			EXECTASK = EXECTASK->nexttcb;
		}
	}
}

extern tcb * readyQueueSearch(void);

int startScheduler_GUARD = 0;

void ROSA_startScheduler(void)
{	
	if (startScheduler_GUARD == 0)
	{	
		TCBLIST = ((tcb *) readyQueueSearch())->nexttcb;
		startScheduler_GUARD = 1;
		ROSA_start();
	}
}
