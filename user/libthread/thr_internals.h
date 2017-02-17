/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <mutex.h>
#include <ureg.h>

// Constants for stack pages
#define ALLOCATED 0
#define FREE 1

// Number of elements allocated each time the size of 'stacks' is increased (to
// avoid doing a system call for that each time a thread is created)
#define CHUNK_SIZE 8

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

void stub(void *(*func)(void *), void *arg);
int thread_fork(void *child_esp);
void multithread_handler(void *arg, ureg_t *ureg);

#endif /* THR_INTERNALS_H */
