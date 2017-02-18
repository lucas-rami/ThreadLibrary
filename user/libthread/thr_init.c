/** @file thr_init.c
 *  @brief This file contains the definition for the thr_init() function
 *  @author akanjani, lramire1
 */

#include <stdlib.h>
#include <thread.h>
#include <mutex.h>
#include <syscall.h>
#include <global_state.h>

// TEMPORARY //
void* addr_exception_stack = NULL;

// Mutex for thread-safe malloc library
extern mutex_t alloc_mutex;

/** @brief Initialize the thread library
 *
 *  This function should be called exactly once before any other
 *  function from the thread library.
 *
 *  @param size The amount of stack space which will be available for
 *  each thread using the thread library
 *
 *  @return Zero on success, a negative number on erro
 */
int thr_init(unsigned int size) {

  // Check validity of argument (check that size is a multiple of page size ?)
  if (size <= 0) {
    return -1;
  }

  // Initialize the the task's global state
  if (spinlock_init(&task.state_lock) < 0 || mutex_init(&task.queue_mutex) < 0) {
    return -1;
  }
  task.stack_size = size;
  task.nb_threads = 1;

  // Initialize the mutex for thread-safe malloc library
  mutex_init(&alloc_mutex);

  // Register new exception handler (no automatic stack growth)
  swexn(addr_exception_stack, multithread_handler, NULL, NULL);

  return 0;
}
