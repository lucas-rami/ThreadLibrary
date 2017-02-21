/** @file mutex_type.h
 *  @brief This file defines the type for mutex.
 *  @author akanjani, lramire1
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

#include <queue.h>
#include <spinlock.h>

typedef struct mutex {
  int init;
  int mutex_state;
  spinlock_t lock;
  generic_queue_t waiting_queue;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
