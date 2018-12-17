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
#define DBG_MODE

#include "dbg/dbg_assert.h"

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
#define T1_STACK_SIZE 32
#define T2_STACK_SIZE 32
#define T3_STACK_SIZE 32
#define T4_STACK_SIZE 32
#define T5_STACK_SIZE 32
#define T6_STACK_SIZE 32
#define T7_STACK_SIZE 32
#define T8_STACK_SIZE 32
#define T9_STACK_SIZE 32
#define T10_STACK_SIZE 32
#define T11_STACK_SIZE 32
#define T12_STACK_SIZE 32

ROSA_taskHandle_t * t1_tcb;
ROSA_taskHandle_t * t2_tcb;
ROSA_taskHandle_t * t3_tcb;
ROSA_taskHandle_t * t4_tcb;
ROSA_taskHandle_t * t5_tcb;
ROSA_taskHandle_t * t6_tcb;
ROSA_taskHandle_t * t7_tcb;
ROSA_taskHandle_t * t8_tcb;
ROSA_taskHandle_t * t9_tcb;
ROSA_taskHandle_t * t10_tcb;
ROSA_taskHandle_t * t11_tcb;
ROSA_taskHandle_t * t12_tcb;

ROSA_semaphoreHandle_t mutex1;
ROSA_semaphoreHandle_t mutex2;
ROSA_semaphoreHandle_t mutex3;
ROSA_semaphoreHandle_t mutex4;
ROSA_semaphoreHandle_t mutex5;
ROSA_semaphoreHandle_t mutex6;

/*************************************************************
 * Task 1
 * Turn LED 1 ON
 ************************************************************/
void task1(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex1);
		ledOn(LED0_GPIO);
		ROSA_delay(511);
		ROSA_semaphoreUnlock(mutex1);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 2
 * Turn LED 0 OFF
 ************************************************************/
void task2(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex1);
		ledOff(LED0_GPIO);
		ROSA_delay(510);
		ROSA_semaphoreUnlock(mutex1);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 3
 * Turn LED 1 ON
 ************************************************************/
void task3(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex2);
		ledOn(LED1_GPIO);
		ROSA_delay(509);
		ROSA_semaphoreUnlock(mutex2);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 4
 * Turn LED 1 OFF
 ************************************************************/
void task4(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex2);
		ledOff(LED1_GPIO);
		ROSA_delay(508);
		ROSA_semaphoreUnlock(mutex2);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 5
 * Turn LED 2 ON
 ************************************************************/
void task5(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex3);
		ledOn(LED2_GPIO);
		ROSA_delay(507);
		ROSA_semaphoreUnlock(mutex3);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 6
 * Turn LED 2 OFF
 ************************************************************/
void task6(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex3);
		ledOff(LED2_GPIO);
		ROSA_delay(506);
		ROSA_semaphoreUnlock(mutex3);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 7
 * Turn LED 3 ON
 ************************************************************/
void task7(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex4);
		ledOn(LED3_GPIO);
		ROSA_delay(505);
		ROSA_semaphoreUnlock(mutex4);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 8
 * Turn LED 3 OFF
 ************************************************************/
void task8(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex4);
		ledOff(LED3_GPIO);
		ROSA_delay(504);
		ROSA_semaphoreUnlock(mutex4);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 9
 * Turn LED 4 ON
 ************************************************************/
void task9(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex5);
		ledOn(LED4_GPIO);
		ROSA_delay(503);
		ROSA_semaphoreUnlock(mutex5);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 10
 * Turn LED 4 OFF
 ************************************************************/
void task10(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex5);
		ledOff(LED4_GPIO);
		ROSA_delay(502);
		ROSA_semaphoreUnlock(mutex5);
		ROSA_yield();
	}
}

/*************************************************************
 * Task 11
 * Turn LED 5 ON
 ************************************************************/
void task11(void)
{
	while(1) {
		usartWriteLine(USART, "task11 Entry\r\n");
		ROSA_semaphoreLock(mutex6);
		usartWriteLine(USART, "task11 locked the mutex\r\n");
		ledOn(LED5_GPIO);
		delay_ms(500);
		//ROSA_delay(501);
		ROSA_semaphoreUnlock(mutex6);
		usartWriteLine(USART, "task11 unlocked the mutex\r\n");
		ROSA_yield();
	}
}

/*************************************************************
 * Task 12
 * Turn LED 5 OFF
 ************************************************************/
void task12(void)
{
	while(1) {
		usartWriteLine(USART, "task11 Entry\r\n");
		ROSA_semaphoreLock(mutex6);
		ledOff(LED5_GPIO);
		delay_ms(500);
		//ROSA_delay(500);
		ROSA_semaphoreUnlock(mutex6);
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
	ROSA_taskCreate(t1_tcb, "tsk1", task1, T1_STACK_SIZE, 1);
	ROSA_taskCreate(t2_tcb, "tsk2", task2, T2_STACK_SIZE, 2);
	ROSA_taskCreate(t3_tcb, "tsk3", task3, T3_STACK_SIZE, 3);
	ROSA_taskCreate(t4_tcb, "tsk4", task4, T4_STACK_SIZE, 4);
	ROSA_taskCreate(t5_tcb, "tsk5", task5, T5_STACK_SIZE, 5);
	ROSA_taskCreate(t6_tcb, "tsk6", task6, T6_STACK_SIZE, 6);
	ROSA_taskCreate(t7_tcb, "tsk7", task7, T7_STACK_SIZE, 7);
	ROSA_taskCreate(t8_tcb, "tsk8", task8, T8_STACK_SIZE, 8);
	ROSA_taskCreate(t9_tcb, "tsk9", task9, T9_STACK_SIZE, 9);
	ROSA_taskCreate(t10_tcb, "tsk10", task10, T10_STACK_SIZE, 10);
	ROSA_taskCreate(t11_tcb, "tsk11", task11, T11_STACK_SIZE, 11);
	ROSA_taskCreate(t12_tcb, "tsk12", task12, T12_STACK_SIZE, 12);
	
	ROSA_semaphoreCreate(&mutex1, 1);
	ROSA_semaphoreCreate(&mutex2, 1);
	ROSA_semaphoreCreate(&mutex3, 1);
	ROSA_semaphoreCreate(&mutex4, 1);
	ROSA_semaphoreCreate(&mutex5, 1);
	ROSA_semaphoreCreate(&mutex6, 1);

	ROSA_startScheduler();

	/* Execution will never return here */
	while(1);
}
