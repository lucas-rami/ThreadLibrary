/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <mutex.h>
#include <ureg.h>

void stub(void *(*func)(void *), void *arg, void* addr_exception_stack);
int thread_fork(void *child_esp);

#endif /* THR_INTERNALS_H */
