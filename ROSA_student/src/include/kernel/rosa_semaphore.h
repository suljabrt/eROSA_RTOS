/**
 * @file rosa_semaphore.h
 * @brief Header file containing declarations for ROSA semaphore functions.
 *
 * Header file containing declarations of ROSA semaphore functions.
 */

/* Tab size: 4 */


#ifndef ROSA_SEMAPHORE_H_
#define ROSA_SEMAPHORE_H_

#include "kernel/rosa_def.h"

/** @struct ROSA_semaphoreHandle_record_t
    @var ROSA_semaphoreHandle_record_t::holder 
		The current holder of the semaphore.
    @var ROSA_semaphoreHandle_record_t::ceiling 
		The priority ceiling of the semaphore.
	@var ROSA_semaphoreHandle_record_t::nextLockedSemaphore 
		Pointer to the next semaphore that is locked, NULL if the semaphore is not locked
*/
typedef struct ROSA_semaphoreHandle_record_t {
	ROSA_taskHandle_t *holder; 
	uint8_t ceiling;
	struct ROSA_semaphoreHandle_record_t * nextLockedSemaphore;
} ROSA_semaphoreHandle;

typedef ROSA_semaphoreHandle * ROSA_semaphoreHandle_t;

/** @struct MaxSem
    @var MaxSem::Ceil 
		The highest ceiling of all locked semaphores.
    @var MaxSem::Mutex 
		The mutex holding the semaphore with the highest ceiling.
*/
typedef struct MaxSem{
	uint8_t Ceil;
	ROSA_semaphoreHandle_t Mutex;
} MaxSemHandle_t;

/***********************************************************
 * Global variable (doesn't have to be because only LOCK uses it)
 ***********************************************************/
/** 
	Stores all locked semaphores
*/
extern ROSA_semaphoreHandle_t  LOCKEDSEMAPHORELIST;

	/** @defgroup semaphores Semaphore management.
		@brief Semaphore create, delete, peek, lock, and unlock.
	  */
	///@{
		
/**
	Create the semaphore with correct values.
	@param handle Double pointer to the semaphore.
	@param ceiling Specific ceiling of the semaphore.
	@return Status code (non-negative if successful, negative otherwise).
	
	Create the semaphore with correct values.
*/
int16_t ROSA_semaphoreCreate(ROSA_semaphoreHandle_t * handle, uint8_t ceiling);

/**
		Delete the specified semaphore.
	@param handle Double pointer to the semaphore.
	@return Status code (non-negative if successful, negative otherwise).
	
	Delete the specified semaphore.
*/
int16_t ROSA_semaphoreDelete(ROSA_semaphoreHandle_t handle);

/**
	Check if the semaphore is already locked.
	@param handle Double pointer to the semaphore.
	@return Status code (1 if successful, 0 otherwise).
	
	Create the semaphore with correct values.
*/
int16_t ROSA_semaphorePeek(ROSA_semaphoreHandle_t handle);

/**
	Lock the specified semaphore.
	@param handle Double pointer to the semaphore.
	@return Status code (non-negative if successful, negative otherwise).
	
	Lock the specified semaphore.
*/
int16_t ROSA_semaphoreLock(ROSA_semaphoreHandle_t handle);

/**
*	Unlock the specified semaphore.
*	@param handle Double pointer to the semaphore.
*	@return Status code (non-negative if successful, negative otherwise).
	
	Unlock the specified semaphore.
*/
int16_t ROSA_semaphoreUnlock(ROSA_semaphoreHandle_t handle);
///@}

#endif /* ROSA_SEMAPHORE_H_ */