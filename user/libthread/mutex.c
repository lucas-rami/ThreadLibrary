/** @file mutex.c
 *  @brief This file contains the definitions for mutex_type.h functions
 *  @author akanjani, lramire1
 */

#include <atomic_ops.h>
#include <mutex.h>
#include <stddef.h>
#include <syscall.h>
#include <thread.h>
#include <thr_internals.h>

#define HELD 0
#define FREE 1
#define MUTEX_UNINITIALIZED 0
#define MUTEX_INITIALIZED 1
#define DONT_RUN 0

/** @brief Enqueue a thread into the waiting list
 *
 *  This function must be called only from mutex_lock(),
 *  otherwise the mutex's state is undefined.
 *
 *  @param mp The mutex
 *  @param thr_id The thread's id of the thread we want to enqueue
 *
 *  @return void
 */
static void thr_enqueue(mutex_t *mp, int thr_id) {

  // Insert the current thread into the waiting queue
  queue_insert_node(&mp->waiting_queue, (void *)thr_id);

  // Release the mutex lock so that other threads may progress
  spinlock_release(&mp->lock);

  // Deschedule the current thread
  int dont_run = DONT_RUN;
  deschedule(&dont_run);
}

/** @brief Dequeue a thread from the waiting list
 *
 *  This function must be called only from mutex_unlock(),
 *  otherwise the mutex's state is undefined.
 *
 *  @param mp The mutex
 *
 *  @return void
 */
static void thr_dequeue(mutex_t *mp) {

  if (mp->waiting_queue.head == NULL) {
    // panic("ILLEGAL Operation! Trying to dequeue from empty queue\n");
  }

  // Make the the list's head thread runnable
  int thr_id = (int)queue_delete_node(&mp->waiting_queue);
  while (make_runnable(thr_id) < 0) {
    continue;
  }
}

/** @brief Initialize a mutex
 *
 *  This function initializes the mutex pointed to by mp.
 *  The effects of using a mutex before it has been initialized,
 *  or of initializing it when it is already initialized and in use
 *  are undefined.
 *
 *  @param mp The mutex to initialize
 *
 *  @return Zero on success, a negative number on error
 */
int mutex_init(mutex_t *mp) {

  if (mp == NULL) {
    return -1;
  }

  spinlock_init(&mp->lock);

  // Make sure we are not interrupted while initializing
  spinlock_acquire(&mp->lock);

  mp->mutex_state = FREE;
  mp->init = MUTEX_INITIALIZED;

  // Initialize the head and tail pointers in the waiting queue of the cvar
  mp->waiting_queue.head = NULL;
  mp->waiting_queue.tail = NULL;

  // Release the lock on the mutex
  spinlock_release(&mp->lock);

  return 0;
}

/** @brief Deactivate a mutex
 *
 *  This function deactivates the mutex pointed to by mp.
 *  It is illegal for an application to use a mutex after it has been destroyed.
 *  It is illegal for an application to attempt to destroy a mutex while it is
 *  locked or threads are trying to acquire it.
 *
 *  @param mp The mutex to deactivate
 *
 *  @return void
 */
void mutex_destroy(mutex_t *mp) {

  if (mp == NULL) {
    // panic("Invalid argument to mutex_destroy\n");
  }

  spinlock_acquire(&mp->lock);

  if (mp->init == MUTEX_UNINITIALIZED) {
    // panic("ILLEGAL operation! Trying to destroy uninitialized mutex\n");
  }

  if (mp->waiting_queue.head != NULL) {
    // panic("ILLEGAL Operation! Trying to destroy mutex while some threads are
    // waiting on it\n");
  }

  mp->init = MUTEX_UNINITIALIZED;

  spinlock_release(&mp->lock);
}

/** @brief Acquire the lock on a mutex
 *
 *  A call to this function ensures mutual exclusion in the
 *  region between itself and a call to mutex unlock().
 *
 *  @param mp The mutex holding the lock we want to acquire
 *
 *  @return void
 */
void mutex_lock(mutex_t *mp) {

  if (mp == NULL) {
    // panic("Invalid argument to mutex_lock\n");
  }

  if (mp->init == MUTEX_UNINITIALIZED) {
    // panic("ILLEGAL Operation! Trying to lock uninitialized mutex\n");
  }

  // Acquire the lock on the mutex
  spinlock_acquire(&mp->lock);

  if (mp->mutex_state == FREE) {
    // No thread is in the critical section, we can lock the mutex
    mp->mutex_state = HELD;
    spinlock_release(&mp->lock);
  } else {
    // If some other thread is in the critical section, enqueue the current one
    thr_enqueue(mp, thr_get_kernel_id(thr_getid()));
    // The thread returning here owns the mutex and may proceed in the critical
    // section
  }
}

/** @brief Gives up the lock on a mutex
 *
 * The calling thread gives up its claim to the lock. It is illegal
 * for an application to unlock a mutex that is not locked.
 *
 *  @param mp The mutex to which the lock belongs
 *
 *  @return void
 */
void mutex_unlock(mutex_t *mp) {

  if (mp == NULL) {
    // panic("Invalid argument to mutex_lock\n");
  }

  if (mp->init == MUTEX_UNINITIALIZED) {
    // panic("ILLEGAL Operation! Trying to lock uninitialized mutex\n");
  }

  // Acquire the lock on the mutex
  spinlock_acquire(&mp->lock);

  if (mp->waiting_queue.head == NULL) {
    // No thread is in the waiting queue, we can unlock the mutex
    mp->mutex_state = FREE;
  } else {
    // Otherwise, run the first thread in the queue
    thr_dequeue(mp);
  }

  // Release the lock on the mutex
  spinlock_release(&mp->lock);
}
