/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 *  @author akanjani, lramire1
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

#include <queue.h>
#include <spinlock.h>

typedef struct cond {
  int init;
  generic_queue_t waiting_queue;
  spinlock_t spinlock;
} cond_t;

#endif /* _COND_TYPE_H */
