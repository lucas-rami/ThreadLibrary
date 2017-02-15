/** @file spinlock.c
 *
 *  @brief This file contains the definitions for functions which can be used
 *   to initilaize, acquire or release a spinlock
 *
 *  @author akanjani, lramire1
 */

#include <spinlock.h>
#include <atomic_ops.h>

#define SPINLOCK_UNLOCKED 0
#define SPINLOCK_LOCKED 1

/** @brief Initializes a spinlock. Sets the initial state to unlocked
 *
 *  @param lock A pointer to the spinlock to initialize
 *
 *  @return Zero on success, a negative number on error
 */
int spinlock_init( spinlock_t *lock ) {

  if ( !lock ) {
    // invalid argument
    return -1;
  }

  // Initialize the lock as unlocked
  *lock = SPINLOCK_UNLOCKED;

  return 0;
}

/** @brief Acquires a spinlock. Atomically changes the state to LOCKED
 *   when no one else is holding the lock. We try in a while loop 
 *   because there is always a chance that someone else acquired a lock
 *   before us and hence we wait for our turn
 *
 *  @param lock A pointer to the spinlock to acquire
 *
 *  @return void
 */
void spinlock_acquire( spinlock_t *lock ) {
  // Try to atomically get the lock and loop until we don't
  while( atomic_exchange( lock, SPINLOCK_LOCKED ) == SPINLOCK_LOCKED );
}

/** @brief Releases a spinlock. Atomically changes the state to UNLOCKED
 *
 *  @param lock A pointer to the spinlock to release
 *
 *  @return void
 */
void spinlock_release( spinlock_t *lock ) {
  // Atomically set the lock as unlocked. No need to loop as we should be
  // able to change the state in our first try
  atomic_exchange( lock, SPINLOCK_UNLOCKED );
}
