/** @file thr_getid.c
 *  @brief This file contains the definition for the thr_getid() and
 * thr_get_kernel_id() function
 *  @author akanjani, lramire1
 */

#include <hash_table.h>
#include <global_state.h>
#include <thr_internals.h>
#include <stdlib.h>

/** @brief Returns the library level thread id of the current thread
 *
 *  @return The library level thread id of the current thread
 */
int thr_getid() {
  tcb_t *tcb = get_tcb();

  // Should not happen
  if (tcb == NULL) {
    //panic("thr_getid: TCB does not exists !\n");
  }

  return tcb->library_tid;
}

/** @brief Returns the kernel id of the thread with given library id
 *
 *  If the thread's TCB does not know the thread's kernel id,
 *  then a gettid() system call is made to collect it.
 *
 *  @return The kernel level id if the requested thread exists, -1 otherwise
 */
int thr_get_kernel_id(int library_tid) {

  tcb_t *tcb = hash_table_get_element(&task.tcbs, &library_tid);

  if (tcb == NULL) {
    return -1;
  }

  if (tcb->kernel_tid == -1) {
    tcb->kernel_tid = gettid();
  }

  return tcb->kernel_tid;
}
