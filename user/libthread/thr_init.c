/** @file thr_init.c
 *  @brief This file contains the definition for the thr_init() function
 *  @author akanjani, lramire1
 */

#include <global_state.h>
#include <hash_table.h>
#include <page_fault_handler.h>
#include <stdlib.h>
#include <syscall.h>
#include <thread.h>

#define NB_BUCKETS_TCB 16

// TEMPORARY //
void *addr_exception_stack = NULL;

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

  // Initialize the task's global state

  // Initialize data structures
  if (queue_init(&task.stack_queue) < 0 || hash_table_init(&task.tcbs, NB_BUCKETS_TCB, find_tcb, hash_function_tcb) < 0) {
    return -1;
  }
  task.tcbs.hash_function = hash_function_tcb;

  // Initialize spinlock/mutexes
  if (spinlock_init(&task.state_lock) < 0 ||
      mutex_init(&task.queue_mutex) < 0) {
    return -1;
  }
  if (mutex_init(&task.tcbs_mutex)) {
    mutex_destroy(&task.queue_mutex);
  }

  // Create TCB for current task
  tcb_t *tcb = malloc(sizeof(tcb_t));
  if (tcb == NULL) {
    mutex_destroy(&task.queue_mutex);
    mutex_destroy(&task.tcbs_mutex);
    return -1;
  }

  tcb->return_status = 0;
  tcb->kernel_tid = gettid();
  tcb->library_tid = 0;
  tcb->stack_low = task.stack_lowest;
  tcb->stack_high = task.stack_highest;

  // Initialize the TCB's spinlock and add the current thread's TCB to the hash
  // table
  if (spinlock_init(&tcb->state_lock) < 0 ||
      hash_table_add_element(&task.tcbs, tcb) < 0) {
    free(tcb);
    mutex_destroy(&task.queue_mutex);
    mutex_destroy(&task.tcbs_mutex);
    return -1;
  }

  // Finish to Initialize the task's global state
  task.stack_size = size;
  task.nb_threads = 1;
  task.tid = 1;

  // Register new exception handler (no automatic stack growth)
  swexn(addr_exception_stack, multithread_handler, NULL, NULL);

  return 0;
}
