/** @file thr_create.c
 *  @brief This file contains the definition for the thr_create() function
 *  @author akanjani, lramire1
 */

#include <stdlib.h>
#include <syscall.h>
#include <thr_internals.h>
#include <global_state.h>
#include <thread.h>

/** @brief Create a new thread to run func(arg)
 *
 *  This function allocates a stack for the new thread and
 *  then invoke the thread_fork system call in an appropriate way.
 *
 *  @param func The function to be ran by the new thread
 *  @param arg The argument to the function
 *
 *  @return The ID of the child thread on success. A negative number on error.
 */
int thr_create(void *(*func)(void *), void *arg) {

  // Check validity of arguments
  if (func == NULL) {
    return -1;
  }

  // Lock the mutex on the task's state before updating it
  mutex_lock(&task_state.mutex);

  // Define boundaries of child's stack
  void *child_stack_high = task_state.stack_lowest;
  void *child_stack_low =
      (void *)((unsigned int)child_stack_high - task_state.stack_size);

  // Go through the list of previously allocated stacks and look for a free page
  int i = 0;
  while (i < task_state.stacks_len && task_state.stacks[i] == ALLOCATED) {
    ++i;
  }

  // We did not found a free page
  if (i == task_state.stacks_len) {
    // Allocate more memory for the 'stacks' array if necessary
    if (task_state.stacks_offset >= task_state.stacks_len) {
      page_state *tmp =
          realloc(task_state.stacks,
                  (task_state.stacks_len + CHUNK_SIZE) * sizeof(page_state));
      if (tmp == NULL) {
        mutex_unlock(&task_state.mutex);
        return -1;
      }

      task_state.stacks = tmp;
      task_state.stacks_len += CHUNK_SIZE;
    }

    i = task_state.stacks_offset;
    ++task_state.stacks_offset;
  }

  // Mark the stack page(s) as ALLOCATED
  task_state.stacks[i] = ALLOCATED;

  // Update the number of threads in the task
  ++task_state.nb_threads;

  // Update the lowest address for this task's thread stacks
  task_state.stack_lowest = child_stack_low;

  // Unlock the mutex on the task's state
  mutex_unlock(&task_state.mutex);

  // Allocate the stack for the child
  if (new_pages(child_stack_low, task_state.stack_size) < 0) {

    // Reset the task's state
    mutex_lock(&task_state.mutex);
    --task_state.nb_threads;
    task_state.stacks[task_state.stacks_offset] = FREE;
    task_state.stack_lowest += task_state.stack_size;
    mutex_unlock(&task_state.mutex);
    return -1;
  }

  // Initialize the child's stack
  unsigned int *child_esp = (unsigned int *)child_stack_high - 2;
  *child_esp = (unsigned int)arg;
  --child_esp;
  *child_esp = (unsigned int)func;
  --child_esp;
  --child_esp;
  *child_esp = (unsigned int)stub;

  // Create the child thread with thread_fork()
  int child_tid;
  if ((child_tid = thread_fork(child_esp)) < 0) {
    // De-allocate stack page(s)
    remove_pages(child_stack_low);

    // Reset the task's state
    mutex_lock(&task_state.mutex);
    --task_state.nb_threads;
    task_state.stacks[task_state.stacks_offset] = FREE;
    task_state.stack_lowest += task_state.stack_size;
    mutex_unlock(&task_state.mutex);

    return -1;
  }

  // Store the child's thread id on the very bottom of its stack
  *((unsigned int *)child_stack_high - 1) = child_tid;

  return child_tid;
}

/** @brief Stub function for newly created thread
 *
 *  This function serves as a safeguard against threads returning from
 *  their body function instead of calling thr_exit().
 *
 *  @param func The function to be ran by the new thread
 *  @param arg The argument to the function
 *  @param stack_high The highest address of the child thread's stack
 *
 *  @return Do not return
 */
void stub(void *(*func)(void *), void *arg) {
  func(arg);
  int status = 0;
  thr_exit(&status);
}
