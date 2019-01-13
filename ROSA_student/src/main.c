
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


ROSA_taskHandle_t task1_handle=NULL;
ROSA_taskHandle_t task2_handle=NULL;
ROSA_taskHandle_t task3_handle=NULL;

void task1(void)
{
	while(1) {
		usartWriteLine(USART, "task 1\n");
		ledToggle(LED0_GPIO);
		//ROSA_taskCreate(&task2_handle, "tsk2", task2, 0x40, 2);
		ROSA_delay(1000);
	}
}


void task2(void)
{
	while(1) {
		usartWriteLine(USART, "task 2\n");
		ledToggle(LED1_GPIO);
		ROSA_taskCreate(&task1_handle, "tsk1", task1, 0x40, 2);
		//ROSA_taskDelete(task2_handle);
		ROSA_delay(1000);
	}
}

void task3(void)
{
	while(1)
	{
		usartWriteLine(USART, "task 3\n");
		ledToggle(LED2_GPIO);
		//ROSA_taskDelete(task1_handle);
		ROSA_delay(1000);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	ROSA_taskCreate(&task1_handle, "tsk1", task1, 0x40, 2);
	ROSA_taskCreate(&task2_handle, "tsk2", task2, 0x40, 2);	
	ROSA_taskCreate(&task3_handle, "tsk3", task3, 0x40, 2);	
	
	//Start the timer
	//timerStart();
	
	ROSA_startScheduler();
	/* Execution will never return here */
	while(1);
}
