/** @file thr_init.c
 *  @brief This file contains the definition for the thr_init() function
 *  @author akanjani, lramire1
 */

#include <stdlib.h>
#include <thread.h>
#include <mutex.h>
#include <syscall.h>
#include <thr_internals.h>
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

  // Initialize the mutex for thread-safe malloc library
  mutex_init(&alloc_mutex);

  int thr_stack_len = (task_state.stack_highest - task_state.stack_lowest) / size;

  // Initialize the task_state_t structure
  task_state.stacks = calloc((thr_stack_len / CHUNK_SIZE) * (CHUNK_SIZE + 1), sizeof(page_state));
  if (task_state.stacks == NULL) {
    mutex_destroy(&alloc_mutex);
    return -1;
  }
  int i;
  for (i = 0 ; i < thr_stack_len ; ++i) {
    task_state.stacks[i] = ALLOCATED;
  }
  task_state.stack_size = size;
  task_state.nb_threads = 1;
  task_state.stacks_len = CHUNK_SIZE;
  task_state.stacks_offset = thr_stack_len;
  mutex_init(&task_state.mutex);

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
  int status = 0;
  task_vanish(status);
}
