#ifndef _GLOBAL_STATE_H_
#define _GLOBAL_STATE_H_

#include <mutex_type.h>
#include <syscall.h>

typedef char page_state;

typedef struct task_state {
  // Size for each thread's stack
  unsigned int stack_size;
  // Lowest address of task's threads stacks (initialized by autostack())
  void *stack_lowest;
  // Lowest address of task's threads stacks (initialized by autostack())
  void *stack_highest;
  // Number of threads running in the task
  unsigned int nb_threads;
  // Array containing the state (FREE or ALLOCATED) for each stack page
  page_state *stacks;
  // Length of stacks array
  unsigned int stacks_len;
  // Position at which to store a new stack in the 'stacks' array
  unsigned int stacks_offset;
  // Mutex for this structure
  mutex_t mutex;
} task_state_t;

task_state_t task_state;

char exception_handler_stack[PAGE_SIZE];

#endif /* _GLOBAL_STATE_H_ */
