/** @file thr_create.c
 *  @brief This file contains the definition for the thr_create() function
 *  @author akanjani, lramire1
 */

#include <thread.h>
#include <stdlib.h>
#include <syscall.h>
#include <thr_internals.h>

/* Assume all of this is declared/assigned elsewhere for now,
 * we will need a data structure to hold these information for each thread */
extern void * stack_high;
extern unsigned int stack_size;

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
int thr_create(void *(*func)(void *), void* arg) {

  // Check validity of arguments
  if (func == NULL) {
    return -1;
  }

  void* child_stack_low = stack_high;
  void* child_stack_high = (void*) ((unsigned int) child_stack_low + stack_size);

  // Allocate the stack for the child
  if (new_pages(child_stack_low, stack_size) < 0 ) {
    return -1;
  }

  // Initialize the child's stack
  unsigned int* child_esp = (unsigned int*) child_stack_high - 2;
  *child_esp = (unsigned int) child_stack_high;
  --child_esp;
  *child_esp = (unsigned int) arg;
  --child_esp;
  *child_esp = (unsigned int) func;
  --child_esp;
  --child_esp;
  *child_esp = (unsigned int) stub;

  // Create the child thread with thread_fork()
  int child_tid;
  if ((child_tid = thread_fork(child_esp)) < 0){
    // If the child thread was not created, de-allocate stack pages and return
    remove_pages(child_stack_low);
    return -1;
  }

  // Store the child's thread id on the very bottom of its stack
  *((unsigned int*) child_stack_high - 1) = child_tid;

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
void stub(void *(*func)(void *), void *arg, void *stack_high) {
  // Need to do something with stack_high: store it in global variable ?

  func(arg);
  int status = 0;
  thr_exit(&status);
}
