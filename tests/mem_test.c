/*****************************************************************************
* Checks if memory allocated in task creation is deallocated properly
* aka if there is no memory leak 
* 
* There are two tasks, which delete and remake each other all the time
*
* SUCCESS: no memory leak
* FAILURE: a memory leak
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
#define T1_STACK_SIZE 1024
#define T2_STACK_SIZE 1024

ROSA_taskHandle_t * t1_tcb;
ROSA_taskHandle_t * t2_tcb;

/*************************************************************
 * Task 1
 ************************************************************/
void task1(void)
{
	while(1) {
		ROSA_taskDelete(t2_tcb);
		ROSA_taskCreate(t2_tcb, "tsk2", task2, T2_STACK_SIZE, 1);
	}
}

/*************************************************************
 * Task 2
 ************************************************************/
void task2(void)
{
	while(1) {
		ROSA_taskDelete(t1_tcb);
		ROSA_taskCreate(t1_tcb, "tsk1", task1, T1_STACK_SIZE, 1);
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
	ROSA_taskCreate(t2_tcb, "tsk2", task2, T2_STACK_SIZE, 1);

	ROSA_startScheduler();

	/* Execution will never return here */
	while(1);
}
