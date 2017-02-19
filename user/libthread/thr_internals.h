/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <mutex.h>
#include <global_state.h>
#include <ureg.h>

void stub(void *(*func)(void *), void *arg, void* addr_exception_stack);
int thread_fork(void *child_esp);

tcb_t* get_tcb();
int find_tcb(void* tcb, void* tid);
unsigned int hash_function_tcb(void* tcb, unsigned int nb_buckets);

#endif /* THR_INTERNALS_H */
