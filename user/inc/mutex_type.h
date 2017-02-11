/** @file mutex_type.h
 *  @brief This file defines the type and functions for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct mutex {
  int init;
  int lock_available;
  int nb_threads_waiting;
} mutex_t;

int mutex_init(mutex_t *mp);
void mutex_destroy(mutex_t *mp);
void mutex_unlock(mutex_t *mp);
void mutex_lock(mutex_t *mp);
int atomic_exchange(int* mutex_lock, int val);
int atomic_cmp_xchg(int* addr_val, int old_val, int new_val);

#endif /* _MUTEX_TYPE_H */
