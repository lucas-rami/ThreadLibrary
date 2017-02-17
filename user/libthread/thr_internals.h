/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <ureg.h>

void stub(void *(*func)(void *), void *arg, void *stack_high);

int thread_fork(void* child_esp);

void multithread_handler(void* arg, ureg_t *ureg);

#endif /* THR_INTERNALS_H */
