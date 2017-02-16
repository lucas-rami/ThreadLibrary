/** @file thread_fork.S
 *  @brief Wrapper for thread fork system call
 *  @author akanjani, lramire1
 */

#include <syscall_int.h>

.global thread_fork

thread_fork:
  int $THREAD_FORK_INT  // Make thread_fork system call
  jne return            // If this is the parent thread, return directly
  movl 4(%esp), %esp    // Otherwise, replace esp with the child's stack address
return:
  ret                   // Return from procedure
