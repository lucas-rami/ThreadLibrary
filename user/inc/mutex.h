/** @file mutex.h
 *  @brief This file defines the prototypes for functions in mutex.c and
 * atomic_ops.S
 *  @author akanjani, lramire1
 */

#ifndef _MUTEX_H
#define _MUTEX_H

#include <mutex_type.h>

int mutex_init(mutex_t *mp);
void mutex_destroy(mutex_t *mp);
void mutex_unlock(mutex_t *mp);
void mutex_lock(mutex_t *mp);
int atomic_exchange(int *mutex_lock, int val);
int atomic_cmp_xchg(int *addr_val, int old_val, int new_val);

#endif /* _MUTEX_H */
