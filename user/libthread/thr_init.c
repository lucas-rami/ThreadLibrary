/** @file thr_init.c
 *  @brief This file contains the definition for the thr_init() function
 *  @author akanjani, lramire1
 */

#include <stdlib.h>
#include <thread.h>
#include <mutex.h>
#include <syscall.h>
#include <thr_internals.h>

// TEMPORARY //
extern int status;
extern void* addr_exception_stack;


// Mutex for thread-safe malloc library
mutex_t alloc_mutex;

// Specifies the amount of stack space available for each thread
unsigned int stack_size;

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

  // Make 'size' visible for all the task's threads
  stack_size = size;

  // Initialize the mutex for thread-safe malloc library
  mutex_init(&alloc_mutex);

  // Register new exception handler (no automatic stack growth)
  swexn(addr_exception_stack, multithread_handler, NULL, NULL);

  return 0;
}

/** @brief Exception handler for multithreaded application
 *
 *  The function simply vanish the current task.
 *
 *  @param arg  Optional argument for exception handler
 *  @param ureg Structure holding information about the exception's cause
 *  and the values of some registers
 *
 *  @return void
 */
void multithread_handler(void* arg, ureg_t *ureg) {
  task_vanish(status);
}
