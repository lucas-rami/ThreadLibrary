/** @file atomic_ops.h
 *  @brief This file defines the prototypes for functions in atomic_ops.S
 *  @author akanjani, lramire1
 */

#ifndef _ATOMIC_OPS_H
#define _ATOMIC_OPS_H

int atomic_exchange(int *mutex_lock, int val);
int atomic_cmp_xchg(int *addr_val, int old_val, int new_val);

#endif /* _ATOMIC_OPS_H */
