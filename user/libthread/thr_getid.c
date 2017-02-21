/** @file thr_getid.c
 *  @brief This file contains the definition for the thr_getid() and
 * thr_get_kernel_id() function
 *  @author akanjani, lramire1
 */

#include <global_state.h>
#include <thr_internals.h>

/** @brief Returns the library level thread id of the current thread
 *
 *  @return The library level thread id of the current thread
 */
int thr_getid() {
  tcb_t *tcb = get_tcb();
  return tcb->library_tid;
}

/** @brief Returns the kernel id of the current thread
 *
 *  If the thread's TCB does not know the kernel id of the current thread,
 *  then a gettid() system call is made to collect it.
 *
 *  @return The kernel level thread id of the current thread
 */
int thr_get_kernel_id() {

  tcb_t *tcb = get_tcb();
  if (tcb->kernel_tid == -1) {
    tcb->kernel_tid = gettid();
  }
  return tcb->kernel_tid;
}
