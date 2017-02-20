/** @file global_state.h
 *  @brief This file defines data structures for the task's state and thread's
 * TCB
 *  @author akanjani, lramire1
 */

#ifndef _GLOBAL_STATE_H_
#define _GLOBAL_STATE_H_

#include <data_structures.h>
#include <mutex.h>
#include <cond.h>
#include <syscall.h>

#define WAITING_ON 0
#define EXITED 1
#define RUNNING 2

typedef struct tcb {

  // Thread's return status
  void* return_status;
  // Thread's id (provided by the kernel)
  // TODO: Not sure we even need it
  int kernel_tid;
  // Thread's if (provided by the thread library)
  int library_tid;
  // Lowest address of thread's stack space
  unsigned int *stack_low;
  // Highest address of thread's stack space
  unsigned int *stack_high;
  // Spinlock for tcb's state access
  spinlock_t state_lock;

  /*------------------------------*/

  // Threads state (either WAITING_ON, EXITED, or RUNNING)
  int thread_state;
  // Condition variable for thr_exit() and thr_join()
  cond_t cond_var_state;
  // Mutex for manipulating the thread's state variable
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
  // Number of threads running in the task
  // TODO: Not sure we really need this...
  unsigned int nb_threads;
  // Thread library tids
  unsigned int tid;
  // Spinlock for task's state access
  spinlock_t state_lock;

  /*------------------------------*/

  // Queue for free stack spaces
  generic_queue_t stack_queue;
  // Mutex for queue access
  mutex_t queue_mutex;

  /*------------------------------*/

  // Data structure holding the TCB of all threads in the task
  generic_hash_table_t tcbs;
  // Mutex for TCBs access
  mutex_t tcbs_mutex;

  /*------------------------------*/

  // TCB of root thread in the task
  /* Even if the root thread exits, we should not reallocate its
   * stack address space for other threads stacks */
  tcb_t* root_tcb;

} task_t;

task_t task;

int exception_handler_stack[PAGE_SIZE];

#endif /* _GLOBAL_STATE_H_ */
