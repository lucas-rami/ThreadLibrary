/** @file thr_exit.c
 *  @brief This file contains the definition for the thr_exit() function
 *  @author akanjani, lramire1
 */

#include <syscall.h>

/** @brief Exit the thread with an exit status
 *
 *  @param status The thread's exit status
 *
 *  @return Do not return
 */

void thr_exit( void *status ) {
  vanish();
}
