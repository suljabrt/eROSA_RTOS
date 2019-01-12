
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
ROSA_taskHandle_t led3_task_handle = NULL;
ROSA_taskHandle_t led5_task_handle = NULL;
ROSA_taskHandle_t led7_task_handle = NULL;

ROSA_semaphoreHandle_t S1;
ROSA_semaphoreHandle_t S2;
ROSA_semaphoreHandle_t S3;


void task1(void)
{
	while(1)
	{
		if(0 == ROSA_semaphoreLock(S3))
		{
			ledOn(LED0_GPIO);
			delay_ms(1000);
		}
		
		if(0 == ROSA_semaphoreLock(S2))
		{
			ledOn(LED1_GPIO);
			delay_ms(1000);
		}
		
		if(0 == ROSA_semaphoreLock(S1))
		{
			ledOn(LED2_GPIO);
			delay_ms(1000);
		}
		
		if(0 == ROSA_semaphoreUnlock(S1))
		{
			ledOff(LED2_GPIO);
			delay_ms(1000);
		}
		
		if(0 == ROSA_semaphoreUnlock(S2))
		{
			ledOff(LED1_GPIO);
			delay_ms(1000);
		}
		
		if(0 == ROSA_semaphoreUnlock(S3))
		{
			ledOff(LED0_GPIO);
			delay_ms(1000);
		}
	}
}


void led3_task(void)
{
	while(1)
	{
		ledToggle(LED3_GPIO);
		ROSA_delay(100);
	}
}

void led5_task(void)
{
	while(1)
	{
		ledToggle(LED5_GPIO);
		ROSA_delay(100);
	}
}

void led7_task(void)
{
	while(1)
	{
		ledToggle(LED7_GPIO);
		ROSA_delay(100);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	ROSA_semaphoreCreate(&S1,7);
	ROSA_semaphoreCreate(&S2,5);
	ROSA_semaphoreCreate(&S3,3);
	
	ROSA_taskCreate(& task1_handle, "tsk1", task1, 0x40, 1);
	ROSA_taskCreate(& led3_task_handle, "led3", led3_task, 0x40, 6);
	ROSA_taskCreate(& led5_task_handle, "led5", led5_task, 0x40, 4);
	ROSA_taskCreate(& led7_task_handle, "led7", led7_task, 0x40, 2);
	
	//Start the timer
	timerStart();
	
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
