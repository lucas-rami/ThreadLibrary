/** @file cond_var.c
 *
 *  @brief This file contains the definitions for condition variable functions
 *   It implements cvar_init, cvar_wait, cvar_signal and cvar_broadcast which
 *   can be used by applications for synchronization
 *
 *  @author akanjani, lramire1
 */

#include <cond.h>
#include <syscall.h>
#include <stdio.h>
#include <thread.h>
#include <thr_internals.h>
#include <assert.h>
#include <mutex.h>

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

  mutex_init( &cv->lock );

  mutex_lock( &cv->lock );

  cv->init = CVAR_INITIALIZED;

  // Initialize the head and tail pointers in the waiting queue of the cvar
  cv->waiting_queue.head = NULL;
  cv->waiting_queue.tail = NULL;

  // Initialize the lock of the cvar
  mutex_unlock( &cv->lock );

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

  assert( cv );

  mutex_lock( &cv->lock );

  assert( cv->init == CVAR_INITIALIZED );

  assert ( cv->waiting_queue.head == NULL );

  cv->init = CVAR_UNINITIALIZED;

  mutex_unlock( &cv->lock );

  mutex_destroy( &cv->lock );
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

  assert ( cv && mp );

  assert ( cv->init == CVAR_INITIALIZED );

  // Make the operation atomic
  mutex_lock( &cv->lock );

  // Add this thread to the waiting queue of this condition variable
  queue_insert_node( &cv->waiting_queue, (void*) thr_get_kernel_id(thr_getid()) );

  // Release the mutex so that other threads can run now
  /* TODO; Could we put mutex_unlock above the call to queue_insert_node() ?
   * We don't want to hold the mutex for too long... */
  mutex_unlock( mp );

  mutex_unlock( &cv->lock );

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

  assert ( cv );

  assert ( cv->init == CVAR_INITIALIZED );

  // Make the whole operation atomic
  mutex_lock( &cv->lock );

  if ( cv->waiting_queue.head != NULL ) {
    // waiting queue is not empty
    void *tid = queue_delete_node( &cv->waiting_queue );

    // Start the thread which was just dequed from the waiting queue
    while (make_runnable( ( int )tid ) < 0) {
      continue;
    }
  }

  mutex_unlock( &cv->lock );
}

/** @brief Wakes up all threads waiting on the condition variable pointed to
 *   by cv
 *
 *  @param cv A pointer to the condition variable
 *
 *  @return void
 */
void cond_broadcast( cond_t *cv ) {

  assert ( cv );

  assert ( cv->init == CVAR_INITIALIZED );

  // Make the whole operation atomic
  mutex_lock( &cv->lock );

  // Loop through the whole waiting queue
  while ( cv->waiting_queue.head != NULL ) {
    // waiting queue is not elockty
    void *tid = queue_delete_node( &cv->waiting_queue );

    // Start the thread which was just dequed
    make_runnable( ( int )tid );
  }

  mutex_unlock( &cv->lock );
}
