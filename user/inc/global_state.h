/** @file global_state.h
 *  @brief This file defines data structures for the task's state and thread's
 * TCB
 *  @author akanjani, lramire1
 */

#ifndef _GLOBAL_STATE_H_
#define _GLOBAL_STATE_H_

#include <mutex.h>
#include <cond_type.h>
#include <syscall.h>
#include <hash_table.h>

#define WAITING_ON 0
#define EXITED 1
#define RUNNING 2

typedef struct tcb {

  // Thread's id (provided by the thread library)
  int library_tid;
  // Lowest address of thread's stack space
  unsigned int *stack_low;
  // Highest address of thread's stack space
  unsigned int *stack_high;
  // Thread's return status
  void* return_status;


  /* library_tid, stack_low and stack_high do not need to be protected by a
   * lock since their value is defined before the thread is created.
   *
   * return_status is written to only by the thread owning this TCB, so
   * no lock is needed for it neither. */

  /*------------------------------*/

  // Thread's id (provided by the kernel)
  int kernel_tid;
  // Condition variable for kernel_tid
  cond_t cond_var_kernel_tid;
  // Mutex for manipulation the kernel_tid field
  mutex_t mutex_kernel_tid;

  /*------------------------------*/

  // Threads state (either WAITING_ON, EXITED, or RUNNING)
  int thread_state;
  // Condition variable for thr_exit() and thr_join()
  cond_t cond_var_state;
  // Mutex for manipulating the thread's state field
  mutex_t mutex_state;

} tcb_t;

typedef struct task {

  // Size for each thread's stack
  unsigned int stack_size;
  // Lowest address of task's threads stacks (initialized by autostack())
  unsigned int *stack_highest;
  // Highest address for (grand)child threads of first thread
  unsigned int* stack_highest_childs;

  /* The above fields do not need to be protected by a lock since
   * their value does not change after thr_init() has returned */
  /*------------------------------*/

  // Lowest address of task's threads stacks (initialized by autostack())
  unsigned int *stack_lowest;
  // Thread library tids
  unsigned int tid;
  // Spinlock for task's state access
  mutex_t state_lock;

  /*------------------------------*/

  // Queue for free stack spaces
  generic_queue_t stack_queue;
  // Mutex for queue access
  mutex_t queue_mutex;

  /*------------------------------*/

  // Data structure holding the TCB of all threads in the task
  generic_hash_table_t tcbs;

  /*------------------------------*/

  // TCB of root thread in the task
  tcb_t* root_tcb;

} task_t;

// GLobal state for the current task
task_t task;

// Page_sized exception stack for root thread in task
int exception_handler_stack[PAGE_SIZE];

#endif /* _GLOBAL_STATE_H_ */
