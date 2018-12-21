/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
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

//Standard library includes
#include <avr32/io.h>

#include "kernel/rosa_semaphore.h"

//Kernel includes
#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
#define T1_STACK_SIZE 256
ROSA_taskHandle_t * t1_tcb;

#define T2_STACK_SIZE 256
ROSA_taskHandle_t * t2_tcb;

/*************************************************************
 * Task2
 * Blink LED1
 ************************************************************/
void task2(void)
{
	while(1) {
		//usartWriteLine(USART, "task 2 entry\r\n");
		ledOn(LED1_GPIO);
		//usartWriteLine(USART, "task 2 delayed\r\n");
		ROSA_delay(500);
		//usartWriteLine(USART, "task 2 woken up!\r\n");
		ledOff(LED1_GPIO);
		ROSA_delay(500);
		ROSA_yield();
	}
}


/*************************************************************
 * Task1
 * Blink LED0
 ************************************************************/
void task1(void)
{	
	while(1) {
		ledOn(LED0_GPIO);
		ROSA_delay(500);
		ledOff(LED0_GPIO);
		ROSA_delay(250);
		ROSA_taskDelete(&t2_tcb);
		ROSA_yield();
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	//Create tasks and install them into the ROSA kernel
	ROSA_taskCreate(&t1_tcb, "tsk1", task1, T1_STACK_SIZE, 3);
	ROSA_taskCreate(&t2_tcb, "tsk2", task2, T2_STACK_SIZE, 2);

	ROSA_startScheduler();

	/* Execution will never return here */
	while(1);
}
