/** @file thr_create.c
 *  @brief This file contains the definition for the thr_create() function
 *  @author akanjani, lramire1
 */

#include <global_state.h>
#include <stdlib.h>
#include <syscall.h>
#include <thr_internals.h>
#include <thread.h>
#include <simics.h>
#include <page_fault_handler.h>

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

  unsigned int *child_stack_low, *child_stack_high;

  // Try to find space for a new stack in the queue
  mutex_lock(&task.queue_mutex);
  child_stack_high = delete_node(&task.stack_queue);
  mutex_unlock(&task.queue_mutex);

  // Define child_stack_low/high and update global state if necessary
  spinlock_acquire(&task.state_lock);
  if (child_stack_high == NULL) {
    child_stack_high = task.stack_lowest - PAGE_SIZE;
    // Need space for : 1 stack + 1 page (exception stack) + 1 page (guardpage)
    task.stack_lowest -= (task.stack_size + 2 * PAGE_SIZE);
  }
  child_stack_low = child_stack_high - task.stack_size - PAGE_SIZE;
  spinlock_release(&task.state_lock);

  // Allocate the stack for the child
  if (new_pages(child_stack_low, (unsigned int) child_stack_high - (unsigned int) child_stack_low) < 0) {
    // If we could not allocate stack space, put them in the queue
    mutex_lock(&task.queue_mutex);
    insert_node(&task.stack_queue, child_stack_high);
    mutex_unlock(&task.queue_mutex);
    return -1;
  }

  // Initialize the child's stack (at lower addresses than the exception stack)
  unsigned int *child_esp =
      (unsigned int *)((unsigned int)child_stack_high - PAGE_SIZE) - 2;
  *child_esp = (unsigned int)child_stack_high; // Address of exception stack
  --child_esp;
  *child_esp = (unsigned int)arg;
  --child_esp;
  *child_esp = (unsigned int)func;
  --child_esp;
  // Empty space for "virtual" stub's calling function
  --child_esp;
  *child_esp = (unsigned int)stub; // Address of stub function

  // Create the child thread with thread_fork()
  int child_tid;
  if ((child_tid = thread_fork(child_esp)) < 0) {
    // De-allocate stack space
    remove_pages(child_stack_high);
    // Put stack space in the queue
    mutex_lock(&task.queue_mutex);
    insert_node(&task.stack_queue, child_stack_high);
    mutex_unlock(&task.queue_mutex);
    return -1;
  }

  // Store the child's thread id on the very bottom of its stack
  // TODO: What happens if the child tries to access it before we run this code
  *((unsigned int *)(child_stack_high - PAGE_SIZE - 1)) = child_tid;

  // Update the number of threads in the task before returning
  spinlock_acquire(&task.state_lock);
  ++task.nb_threads;
  spinlock_release(&task.state_lock);

  return child_tid;
}

/** @brief Stub function for newly created thread
 *
 *  This function serves as a safeguard against threads returning from
 *  their body function instead of calling thr_exit(). It also allows
 *  us to register an exception handler for the newly created thread.
 *
 *  @param func The function to be ran by the new thread
 *  @param arg The argument to the function
 *  @param stack_high The highest address of the child thread's stack
 *
 *  @return Do not return
 */
void stub(void *(*func)(void *), void *arg, void *addr_exception_stack) {

  swexn(addr_exception_stack, multithread_handler, NULL, NULL);

  void* ret = func(arg);
  thr_exit(ret);
}
