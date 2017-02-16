/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#define NORETURN __attribute__((__noreturn__))

void stub(void *(*func)(void *), void *arg, void *stack_high) NORETURN;

#endif /* THR_INTERNALS_H */
