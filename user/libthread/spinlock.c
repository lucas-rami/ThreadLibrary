/** @file spinlock.c
 *
 *  @brief This file contains the definitions for functions which can be used
 *   to initilaize, acquire or release a spinlock
 *
 *  @author akanjani, lramire1
 */

#include <spinlock.h>
#include <atomic_ops.h>
#include <syscall.h>
#include <spinlock_asm.h>
#include <thr_internals.h>
#include <simics.h>
#include <thread.h>

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

  lock->prev = 0;

  lock->next_ticket = 1;

  lock->curr_thread = -1;

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

  int j = 1;
  int my_ticket = atomic_add_and_update( &lock->next_ticket, j );

  while( ( lock->prev + 1 ) != my_ticket ) {
    yield( -1 );
  }
  // lock->curr_thread = thr_get_kernel_id( thr_getid() );
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
  /*if ( lock->curr_thread == -1 ) {
    // panic
    lprintf( "ERROR! Release on a non acquired spinlock\n" );
  }*/
  lock->prev++;
}
