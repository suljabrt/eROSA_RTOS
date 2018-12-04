/*
 * rosa_semaphore.c
 *
 * Created: 12/2/2018 2:46:35 PM
 *  Author: haris
 */ 

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_semaphore.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/***********************************************************
 * Global variable that contain the list of Semaphores that
 * 	that are currently locked
 ***********************************************************/
ROSA_semaphoreHandle_t * CREATEDSEMAPHORELIST; //list of pointers to the created semaphores

/***********************************************************
 * ROSA_semaphoreCreate
 *
 * Comment:
 * 	Create a semaphore
 *
 **********************************************************/
int16_t ROSA_semaphoreCreate(ROSA_semaphoreHandle_t ** mutex, uint8_t ceiling) {
	*mutex = (ROSA_semaphoreHandle_t *) calloc(1,sizeof(ROSA_semaphoreHandle_t));
	(*mutex)->holder = NULL;
	(*mutex)->ceiling = ceiling;
	(*mutex)->nextSemaphore=NULL;
	if (CREATEDSEMAPHORELIST=NULL)
	{
		CREATEDSEMAPHORELIST=(*mutex);
	}
	else
	{
		ROSA_semaphoreHandle_t * it; //finding the last semaphore in the list and changing its nextSemaphore field to point to the just created semaphore
		while(it->nextSemaphore!=NULL)
		{
			it=it->nextSemaphore;
		}
		it->nextSemaphore=(*mutex);
	}
	
	return 0;
}
/***********************************************************
 * ROSA_semaphoreDelete
 *
 * Comment:
 * 	Delete a semaphore, if deletion is successful return 0, otherwise -1
 *
 **********************************************************/
int16_t ROSA_semaphoreDelete(ROSA_semaphoreHandle_t ** mutex) {
	if ((*mutex)->holder == NULL) {		
		ROSA_semaphoreHandle_t * it; //find the previous semaphore and chande its nextSemaphore field properly
		it=CREATEDSEMAPHORELIST;
		while (it->nextSemaphore!=(*mutex))
		{
			it=it->nextSemaphore;
		}
		it->nextSemaphore=(*mutex)->nextSemaphore;		
		free((*mutex));
		return 0;
	}
	else
	return - 1;
}
/***********************************************************
 * ROSA_semaphorePeek
 *
 * Comment:
 * 	Check if semaphore is locked, return 1 if its unlocked, 0 otherwise
 *
 **********************************************************/
int16_t ROSA_semaphorePeek(ROSA_semaphoreHandle_t ** mutex) {
	return ((*mutex)->holder == NULL) ? 1 : 0;
}
/***********************************************************
 * ROSA_semaphoreLock
 *
 * Comment:
 * 	Lock the semaphore, return nonnegative value if successful, otherwise return negative value
 *
 **********************************************************/
int16_t ROSA_semaphoreLock(ROSA_semaphoreHandle_t ** mutex) {
	
	while ((*mutex)->holder != NULL)
		ROSA_yield();
	
	(*mutex)->holder = EXECTASK;
	if (EXECTASK->priority < (*mutex)->ceiling)
	{
		EXECTASK->priority=(*mutex)->ceiling; //IPCP priority inheritance
		PA[EXECTASK->priority]=EXECTASK;//move this task to the proper ready queue
	}	
	
	return 0;
}
/***********************************************************
 * ROSA_semaphoreUnlock
 *
 * Comment:
 * 	Unlock the semaphore, return nonnegative value if successful, otherwise return negative value
 *
 **********************************************************/
int16_t ROSA_semaphoreUnlock(ROSA_semaphoreHandle_t ** mutex) {
	(*mutex)->holder = NULL;
	
		
	return 0;
}
