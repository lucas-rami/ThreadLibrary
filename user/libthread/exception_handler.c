/** @file exception_handler.c
 *  @brief This file contains the definition for the multithread_handler() function
 *  @author akanjani, lramire1
 */

 #include <syscall.h>

 /** @brief Exception handler for multithreaded application
  *
  *  The function simply vanish the current task.
  *
  *  @param arg  Optional argument for exception handler
  *  @param ureg Structure holding information about the exception's cause
  *  and the values of some registers
  *
  *  @return void
  */
 void multithread_handler(void* arg, ureg_t *ureg) {
   task_vanish(-1);
 }
