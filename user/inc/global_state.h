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

  // Thread's id (provided by the thread library)
  int library_tid;
  // Lowest address of thread's stack space
  unsigned int *stack_low;
  // Highest address of thread's stack space
  unsigned int *stack_high;
  // Thread's return status
  void* return_status;
  // Thread's id (provided by the kernel)
  int kernel_tid;

  /* library_tid, stack_low and stack_high do not need to be protected by a
   * lock since their value is defined before the thread is created.
   *
   * return_status is written to only by the thread owning this TCB, so
   * no lock is needed for it neither.
   *
   * kernel_tid may be written to by multiple threads. They will change the
   * value for this field (initialized as -1 by thr_create()) at most once,
   * and in the case that multiple threads change this field, they will replace
   * it by the same value. Hence we do not need a lock to protect it. */

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
  tcb_t* root_tcb;

} task_t;

// GLobal state for the current task
task_t task;

// Page_sized exception stack for root thread in task
int exception_handler_stack[PAGE_SIZE];

#endif /* _GLOBAL_STATE_H_ */
