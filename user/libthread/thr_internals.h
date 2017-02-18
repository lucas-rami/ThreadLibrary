/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <mutex.h>
#include <ureg.h>

// Constants for stack pages
#define ALLOCATED 0
#define FREE 1

// Number of elements allocated each time the size of 'stacks' is increased (to
// avoid doing a system call for that each time a thread is created)
#define CHUNK_SIZE 8

void stub(void *(*func)(void *), void *arg);
int thread_fork(void *child_esp);
void multithread_handler(void *arg, ureg_t *ureg);

#endif /* THR_INTERNALS_H */
