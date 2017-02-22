/** @file rwlock_type.h
 *  @brief This file defines the type for reader/writer locks.
 */

#ifndef _RWLOCK_TYPE_H
#define _RWLOCK_TYPE_H

#include <mutex_type.h>
#include <cond_type.h>

typedef struct rwlock {
  int waiting_readers, active_readers, waiting_writers, active_writers;
  int curr_op, init;
  mutex_t lock;
  cond_t read_cvar, write_cvar;
} rwlock_t;

#endif /* _RWLOCK_TYPE_H */
