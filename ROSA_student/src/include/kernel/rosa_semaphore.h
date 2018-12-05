/*
 * rosa_semaphore.h
 *
 * Created: 12/2/2018 2:52:00 PM
 *  Author: haris
 */ 


#ifndef ROSA_SEMAPHORE_H_
#define ROSA_SEMAPHORE_H_

#include "kernel/rosa_def.h";




typedef struct ROSA_semaphoreHandle_record_t {
	ROSA_taskHandle_t *holder; 
	uint8_t ceiling;
	struct ROSA_semaphoreHandle_record_t * nextLockedSemaphore;
} ROSA_semaphoreHandle_t;

/***********************************************************
 * Global variable (doesnt have to be because only LOCK uses it)
 ***********************************************************/
extern ROSA_semaphoreHandle_t * LOCKEDSEMAPHORELIST;


int16_t ROSA_semaphoreCreate(ROSA_semaphoreHandle_t ** handle, uint8_t ceiling);
int16_t ROSA_semaphoreDelete(ROSA_semaphoreHandle_t ** handle);
int16_t ROSA_semaphorePeek(ROSA_semaphoreHandle_t ** handle);
int16_t ROSA_semaphoreLock(ROSA_semaphoreHandle_t ** handle);
int16_t ROSA_semaphoreUnlock(ROSA_semaphoreHandle_t ** handle);


#endif /* ROSA_SEMAPHORE_H_ */