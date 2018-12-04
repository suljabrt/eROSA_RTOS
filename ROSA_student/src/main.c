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

//Kernel includes
#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
#define T1_STACK_SIZE 32
//static int t1_stack[T1_STACK_SIZE];
//static tcb t1_tcb;
ROSA_taskHandle_t * t1_tcb;

#define T2_STACK_SIZE 32
#define T3_STACK_SIZE 32
//static int t2_stack[T2_STACK_SIZE];
//static tcb t2_tcb;
ROSA_taskHandle_t * t2_tcb;
ROSA_taskHandle_t * t3_tcb;
ROSA_taskHandle_t * t4_tcb;

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	while(1) {
		ledOn(LED0_GPIO);
		delay_ms(350);
		ledOff(LED0_GPIO);
		delay_ms(350);
		//ledOff(LED1_GPIO);
		//delay_ms(350);
		ROSA_yield();
	}
}

void task4(void)
{
	while(1) {
		ledOn(LED4_GPIO);
		delay_ms(350);
		ledOff(LED4_GPIO);
		//ledOff(LED1_GPIO);
		delay_ms(350);
		ROSA_yield();
	}
}

void task3(void)
{
	while(1) {
		ledOn(LED2_GPIO);
		delay_ms(350);
		ledOff(LED2_GPIO);
		//ledOff(LED1_GPIO);
		delay_ms(350);
		ROSA_taskDelete(&t3_tcb);
		ROSA_yield();
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task2(void)
{
	while(1) {
		ledOn(LED1_GPIO);
		delay_ms(350);
		ledOff(LED1_GPIO);
		delay_ms(350);
		ROSA_taskCreate(&t3_tcb, "tsk3", task3, T3_STACK_SIZE, 5);
		ledOn(LED1_GPIO);
		ROSA_taskDelete(&t2_tcb);
	//	ledOn(LED1_GPIO);
	//	delay_ms(350);
		ledOff(LED1_GPIO);
		//ledOff(LED1_GPIO);
		delay_ms(350);
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
	ROSA_taskCreate(&t1_tcb, "tsk1", task1, T1_STACK_SIZE, 4);
	ROSA_taskCreate(&t2_tcb, "tsk2", task2, T2_STACK_SIZE, 4);
	//ledOn(LED1_GPIO);
/*	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	ROSA_tcbInstall(&t2_tcb);*/

	//Start the ROSA kernel
	//TCBLIST = PA[2]->nexttcb;
	//ROSA_start();
	ROSA_startScheduler();
	//ledOn(LED1_GPIO);
	/* Execution will never return here */
	while(1);
}
