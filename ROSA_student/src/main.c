/*****************************************************************************
This test case aims to test basic IPCP functionality. Three tasks T1, T2, and T3
are created and their priorities are 1,2 and 3. Task 1 locks semaphore3 and turns
ON LED3, then task2 tries to execute but cannot preempt task1. Task3 then tries to execute
and it takes semaphore1 and turns on LED1, then turns it off after 500ms and
unlocks the semaphore1. Task1 then locks semaphore2 and turns on LED2 for 500ms
then unlocks semaphore2. Later it turns off LED3 and releases semaphore3. Task2 then
executes: locks semaphore2 and turn on LED2, after 500ms locks semaphore3 and turns on 
LED3, 500ms later unlocks semaphore3 and turns off LED3 and 500ms after unlocks semaphore2
and turns off LED2.
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
#define T1_STACK_SIZE 32
#define T2_STACK_SIZE 32
#define T3_STACK_SIZE 32


ROSA_taskHandle_t * t1_tcb;
ROSA_taskHandle_t * t2_tcb;
ROSA_taskHandle_t * t3_tcb;


ROSA_semaphoreHandle_t mutex1;
ROSA_semaphoreHandle_t mutex2;
ROSA_semaphoreHandle_t mutex3;



void task1(void)
{
	while(1) {
		ROSA_semaphoreLock(mutex3);
		ledOn(LED2_GPIO);
		delay_ms(2100);
		ROSA_semaphoreLock(mutex2);
		ledOn(LED1_GPIO);
		delay_ms(500);
		ledOff(LED1_GPIO);
		ROSA_semaphoreUnlock(mutex2);
		ledOff(LED2_GPIO);
		ROSA_semaphoreUnlock(mutex3);
		ROSA_yield();
	}
}


void task2(void)
{
	while(1) {
		ROSA_delay(500);
		ROSA_semaphoreLock(mutex2);
		ledOn(LED1_GPIO);
		delay_ms(500);
		ROSA_semaphoreLock(mutex3);
		ledOn(LED2_GPIO);
		delay_ms(500);
		ledOff(LED2_GPIO);
		ROSA_semaphoreUnlock(mutex3);
		delay_ms(500);
		ledOff(LED1_GPIO);
		ROSA_semaphoreUnlock(mutex2);
		ROSA_yield();
	}
}


void task3(void)
{
	while(1) {
		ROSA_delay(1500);
		ROSA_semaphoreLock(mutex1);
		ledOn(LED0_GPIO);
		delay_ms(500);
		ledOff(LED0_GPIO);
		ROSA_semaphoreUnlock(mutex1);
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
	
	ROSA_semaphoreCreate(&mutex1, 3);
	ROSA_semaphoreCreate(&mutex2, 2);
	ROSA_semaphoreCreate(&mutex3, 2);
	
	ROSA_startScheduler();
	

	/* Execution will never return here */
	while(1);
}
