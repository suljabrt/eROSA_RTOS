/*****************************************************************************
* Checks if a delayed task can be properly deleted
* 
* There are two tasks, task one delays itself for 1000 ticks, 
* task two tries to delete task one while it is delayed
* 
* SUCCESS: LED1_GPIO turns on while LED0_GPIO is on
* FAILURE: LED0_GPIO turns off
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

ROSA_taskHandle_t * t1_tcb;
ROSA_taskHandle_t * t2_tcb;

/*************************************************************
 * Task 1
 * Turn LED 0 ON
 ************************************************************/
void task1(void)
{
	while(1) {
		ledOn(LED0_GPIO);
		ROSA_delay(1000);
		ledOff(LED0_GPIO);
	}
}

/*************************************************************
 * Task 2
 * Turn LED 0 OFF
 ************************************************************/
void task2(void)
{
	while(1) {
		ROSA_taskDelete(t1_tcb);
		ledOn(LED1_GPIO);
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
	ROSA_taskCreate(t1_tcb, "tsk1", task1, T1_STACK_SIZE, 2);
	ROSA_taskCreate(t2_tcb, "tsk2", task2, T2_STACK_SIZE, 1);

	ROSA_startScheduler();

	/* Execution will never return here */
	while(1);
}
