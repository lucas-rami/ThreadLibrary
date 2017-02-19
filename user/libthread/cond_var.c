/** @file cond_var.c
 *
 *  @brief This file contains the definitions for condition variable functions
 *   It implements cvar_init, cvar_wait, cvar_signal and cvar_broadcast which
 *   can be used by applications for synchronization
 *
 *  @author akanjani, lramire1
 */

#include <cond_type.h>
#include <cond.h>
#include <queue.h>
#include <stddef.h>
#include <syscall.h>
#include <stdio.h>

#define CVAR_INITIALIZED 1
#define CVAR_UNINITIALIZED 0
#define DONT_RUN 0

/** @brief Initializes a condition variable
 *
 *  This function initializes the condition variable pointed to by cv.
 *  The effects of using a condition variable before it has been initialized,
 *  or of initializing it when it is already initialized and in use
 *  are undefined.
 *
 *  @param cv The condition variable to initialize
 *
 *  @return Zero on success, a negative number on error
 */
int cond_init( cond_t *cv ) {

  if ( !cv ) {
    return -1;
  }

  spinlock_init( &( cv->spinlock ) );

  spinlock_acquire( &( cv->spinlock ) );

  cv->init = CVAR_INITIALIZED;

  // Initialize the head and tail pointers in the waiting queue of the cvar
  cv->waiting_queue.head = NULL;
  cv->waiting_queue.tail = NULL;

  // Initialize the spinlock of the cvar
  spinlock_release( &( cv->spinlock ) );
  return 0;
}

/** @brief Destroys a condition variable
 *
 *  This function deactivates the condition variable pointed to by cv.
 *  It is illegal for an application to use a condition variable after it has
 *  been destroyed (unless and until it is later re-initialized). It is
 *  illegal for an application to invoke cond destroy() on a condition
 *  variable while threads are blocked waiting on it.
 *
 *  @param cv A pointer to the condition variable to deactivate
 *
 *  @return void
 */
void cond_destroy( cond_t *cv ) {
  if ( !cv ) {
    // panic( "Invalid argument to cond_destroy\n" );
  }

  spinlock_acquire( &cv->spinlock );

  if ( cv->init == CVAR_UNINITIALIZED ) {
    // panic( "ILLEGAL Operation! Cannot destroy the condition variable. Reason:"
    //  "It is not initialized.\n" );
  }

  if ( cv->waiting_queue.head != NULL ) {
    // Shouldnt happen
    // panic( "ILLEGAL Operation! Cannot destroy the condition variable. Reason:"
    //  "There are threads blocked waiting for this condition variable.\n" );
  }

  cv->init = CVAR_UNINITIALIZED;

  spinlock_release( &cv->spinlock );
}

/** @brief Waits for a condition to be true associated with cv
 *
 *  It allows a thread to wait for a condition and release the associated
 *  mutex that it needs to hold to check that condition. The calling thread
 *  blocks, waiting to be signaled. The blocked thread may be awakened by a
 *  cond signal() or a cond broadcast(). Upon return from cond wait(), *mp
 *  has been re-acquired on behalf of the calling thread.
 *
 *  @param cv A pointer to the condition variable
 *  @param mp A pointer to the mutex held by the thread
 *
 *  @return void
 */
void cond_wait( cond_t *cv, mutex_t *mp ) {

  if ( !cv || !mp ) {
    // panic( "Invalid argument to cond_wait\n" );
  }

  if ( cv->init == CVAR_UNINITIALIZED ) {
    // panic( "ILLEGAL Operation! Cannot wait on this condition variable. Reason:"
    //  "It is not initialized.\n" );
  }
  // Make the operation atomic
  // TODO: Do we really need this lock
  spinlock_acquire( &cv->spinlock );

  // Add this thread to the waiting queue of this condition variable
  queue_insert_node( &cv->waiting_queue, (void*)gettid() );

  // Release the mutex so that other threads can run now
  mutex_unlock( mp );

  spinlock_release( &cv->spinlock );

  // Tell the scheduler to not run this thread
  int dont_run = DONT_RUN;
  deschedule( &dont_run );

  // Take the mutex before leaving cvar_wait
  mutex_lock( mp );

}

/** @brief Wakes up a thread waiting on the condition variable pointed to
 *   by cv, if one exists
 *
 *  @param cv A pointer to the condition variable
 *
 *  @return void
 */
void cond_signal( cond_t *cv ) {

  if ( !cv ) {
    // panic( "Invalid argument to cond_signal\n" );
  }

  if ( cv->init == CVAR_UNINITIALIZED ) {
    // panic( "ILLEGAL Operation! Cannot signal this condition variable. Reason:"
    //  "It is not initialized.\n" );
  }
  // Make the whole operation atomic
  spinlock_acquire( &cv->spinlock );

  if ( cv->waiting_queue.head != NULL ) {
    // waiting queue is not elockty
    void *tid = queue_delete_node( &cv->waiting_queue );

    if ( !tid ) {
      // Queue is elockty or something went wrong
      printf( "Error while deleting from the waiting queue\n" );
    }

    // Start the thread which was just dequed from the waiting queue
    while (make_runnable( ( int )tid ) < 0) {
      continue;
    }
  }

  spinlock_release( &cv->spinlock );
}

/** @brief Wakes up all threads waiting on the condition variable pointed to
 *   by cv
 *
 *  @param cv A pointer to the condition variable
 *
 *  @return void
 */
void cond_broadcast( cond_t *cv ) {

  if ( !cv ) {
    // panic( "Invalid argument to cond_broadcast\n" );
  }

  if ( cv->init == CVAR_UNINITIALIZED ) {
    // panic( "ILLEGAL Operation! Cannot broadcast this condition variable."
    //  " Reason: It is not initialized.\n" );
  }
  // Make the whole operation atomic
  spinlock_acquire( &cv->spinlock );

  // Loop through the whole waiting queue
  while ( cv->waiting_queue.head != NULL ) {
    // waiting queue is not elockty
    void *tid = queue_delete_node( &cv->waiting_queue );

    if ( !tid ) {
      // Something went wrong
      printf( "Error while deleting from the waiting queue\n" );
    }

    // Start the thread which was just dequed
    make_runnable( ( int )tid );
  }

  spinlock_release( &cv->spinlock );
}
