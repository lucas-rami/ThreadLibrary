/** @file mutex.c
 *  @brief This file contains the definitions for mutex_type.h functions
 *  @author akanjani, lramire1
 */

#include <mutex.h>

#define LOCKED_VAL 0
#define UNLOCKED_VAL 1

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

  if (mp->init) {
    return -1;
  }

  mp->lock_available = UNLOCKED_VAL;
  mp->nb_threads_waiting = 0;
  mp->init = 1;
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

  while (mp->init) {

    // Wait till we get the lock
    while (!mp->init || !atomic_exchange(&mp->lock_available, LOCKED_VAL)) {
      continue;
    }
    // If no other thread is waiting, deactivate the mutex
    if (mp->nb_threads_waiting == 0) {
      mp->init = 0;
    }
    // Unlock the mutex
    atomic_exchange(&mp->lock_available, UNLOCKED_VAL);
  }
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

  // Increment the number of threads waiting
  int nb_threads = mp->nb_threads_waiting;
  while (
      !atomic_cmp_xchg(&mp->nb_threads_waiting, nb_threads, nb_threads + 1)) {
    nb_threads = mp->nb_threads_waiting;
  }

  // Wait till we get the lock
  while (!mp->init || !atomic_exchange(&mp->lock_available, LOCKED_VAL)) {
    continue;
  }

  // Decrement the number of threads waiting
  nb_threads = mp->nb_threads_waiting;
  while (
      !atomic_cmp_xchg(&mp->nb_threads_waiting, nb_threads, nb_threads - 1)) {
    nb_threads = mp->nb_threads_waiting;
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
  // This condition should always evaluate to true
  if (mp->init && !mp->lock_available) {
    atomic_exchange(&mp->lock_available, UNLOCKED_VAL);
  }
}
