/** @file global_state.h
 *  @brief This file defines a data structure for the task's state
 *  @author akanjani, lramire1
 */

#ifndef _GLOBAL_STATE_H_
#define _GLOBAL_STATE_H_

#include <mutex_type.h>
#include <syscall.h>

typedef char page_state;

typedef struct task {
  // Size for each thread's stack
  unsigned int stack_size;
  // Lowest address of task's threads stacks (initialized by autostack())
  unsigned int *stack_lowest;
  // Lowest address of task's threads stacks (initialized by autostack())
  unsigned int *stack_highest;
  // Number of threads running in the task
  unsigned int nb_threads;
  // Spinlock for task's state access
  spinlock_t state_lock;

  // Queue for free stack spaces
  generic_queue_t stack_queue;
  // Mutex for queue access
  mutex_t queue_mutex;

} task_t;

task_t task;

char exception_handler_stack[PAGE_SIZE];

#endif /* _GLOBAL_STATE_H_ */
