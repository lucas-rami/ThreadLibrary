/** @file sem.c
 *
 *  @brief This file contains the definitions for sempahore functions
 *   It implements sem_init, sem_wait, sem_signal and sem_destroy which
 *   can be used by applications for synchronization
 *
 *  @author akanjani, lramire1
 */

#include <sem_type.h>
#include <sem.h>
#include <mutex.h>
#include <cond.h>
#include <simics.h>

#define SEM_INITIALIZED 1
#define SEM_UNINITIALIZED 0

/** @brief Initializes a semaphore
 *
 *  This function initializes the semaphore pointed to by sem. 
 *  The number of resources available to this semaphore are initialized to the
 *  value count passed as a parameter to this function.
 *  The effects of using a semaphore before it has been initialized,
 *  or of initializing it when it is already initialized and in use
 *  are undefined.
 *
 *  @param sem The semaphore to initialize
 *  @param count The number of available resources for this semaphore
 *
 *  @return Zero on success, a negative number on error
 */
int sem_init( sem_t *sem, int count ) {

  if ( !sem || count <= 0 ) {
    // Invalid arguments
    return -1;
  }

  // Initialize the mutex and the condition variable for this semaphore
  if ( mutex_init( &sem->lock ) < 0 || cond_init( &sem->cvar ) < 0 ) {
    // failed to init the mutex for this semaphore
    return -1;
  }

  // Lock the mutex to ensure atomicity
  mutex_lock( &sem->lock );

  // TODO: Do we need this? we can set the value of available resources as -1
  // instead and use that to figure out if the semaphore is init or not.
  // Initialize the semaphore state
  sem->init = SEM_INITIALIZED;

  // Initialize the number of available resources to the count paramter.
  sem->available_resources = count;

  // Initialize the number of threads that have been woken up but are yet to 
  // execute to 0
  sem->wokenup_waiting = 0;

  // Unlock the mutex as the initialization is done
  mutex_unlock( &sem->lock );

  return 0;
}

/** @brief Waits for a resource associated with sem to be available
 *
 *  The semaphore wait function allows a thread to decrement a semaphore value,
 *  and may cause it to block indefinitely until it is legal to 
 *  perform the decrement.
 *
 *  @param sem A pointer to the semaphore
 *
 *  @return void
 */
void sem_wait( sem_t *sem ) {

  if ( !sem ) {
    // Invalid argument
    return;
  }

  if ( sem->init != SEM_INITIALIZED ) {
    // TODO: panic?
    // The semaphore isn't initialized. ERROR!
    lprintf( "Sem wait on uninitialized semaphore\n" );
    return;
  }

  // Take the lock to ensure atomicity
  mutex_lock( &sem->lock );

  // Decrement the number of available resources
  sem->available_resources--;

  if ( sem->available_resources < 0 ) {
    // All the available resources are being used as of now and there is
    // at least one thread waiting to get the resource
    do {

      // Wait for the condition variable to be signaled. Wait for a thread
      // to give up resources
      cond_wait( &sem->cvar, &sem->lock );

    } while ( sem->wokenup_waiting < 1 );

    // As one of the woken up threads now gets to run, decrement the number of
    // threads woken up but not yet executed
    sem->wokenup_waiting--;
  }

  // Release the lock as we are done
  mutex_unlock( &sem->lock );
}

/** @brief This function wakes up a thread waiting on the semaphore pointed 
 *   to by sem, if one exists, and updates the semaphore value regardless.
 *
 *  @param sem A pointer to the semaphore
 *
 *  @return void
 */
void sem_signal( sem_t *sem ) {

  if ( !sem ) {
    // Invalid parameter
    return;
  }

  if ( sem->init != SEM_INITIALIZED ) {
    // TODO: panic?
    // Sem signal on an uninitialized semaphore. ERROR!
    lprintf( "Sem wait on uninitialized semaphore\n" );
    return;
  }

  // Take the lock to ensure atomicity
  mutex_lock( &sem->lock );

  // Increment the number of resources available
  sem->available_resources++;

  // TODO: Should this be <= instead?
  if ( sem->available_resources < 0 ) {
    // There is at least one thread waiting for a resource

    // Increment the number of threads woken up till now
    sem->wokenup_waiting++;

    // Signal the condition variable so that any thread waiting on this
    // condition variable gets to run
    cond_signal( &sem->cvar );
  }

  // Release the lock as we are done
  mutex_unlock( &sem->lock );
}

/** @brief Destroys a semaphore
 *
 *  This function deactivates the semaphore pointed to by sem. Effects of 
 *  using a semaphore after it has been destroyed may be undefined.
 *  It is illegal for an application to use a semaphore after it has been 
 *  destroyed (unless and until it is later re-initialized). It is illegal 
 *  for an application to invoke sem destroy() on a semaphore while threads 
 *  are waiting on it.
 *
 *  @param cv A pointer to the condition variable to deactivate
 *
 *  @return void
 */
void sem_destroy( sem_t *sem ) {

  if ( !sem ) {
    // Invalid parameter
    return;
  }

  if ( sem->init != SEM_INITIALIZED ) {
    // TODO: panic?
    // Sem destory on an uninitialized semaphore. ERROR!
    lprintf( "Sem wait on uninitialized semaphore\n" );
    return;
  }

  // Take the lock to ensure atomicity
  mutex_lock( &sem->lock );

  if ( sem->available_resources < 0 ) {
    mutex_unlock( &sem->lock );
    // PANIC!!!! This is illegal. There are threads waiting on this semaphore
  }

  // Set the semaphore state to uninitialized
  sem->init = SEM_UNINITIALIZED;

  // Set the number of available resources less than 0
  sem->available_resources = -1;

  // Reset the state capturing the number of threads woken up but not yet run
  sem->wokenup_waiting = 0;

  // Destroy the condition variable
  cond_destroy( &sem->cvar );

  // Release the lock as we are done
  mutex_unlock( &sem->lock );

  // Destroy the mutex
  mutex_destroy( &sem->lock );
}
