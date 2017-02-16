/** @file atomic_ops.h
 *  @brief This file defines the prototypes for functions in atomic_ops.S
 *  @author akanjani, lramire1
 */

#ifndef _ATOMIC_OPS_H
#define _ATOMIC_OPS_H

int atomic_exchange(int *mutex_lock, int val);

#endif /* _ATOMIC_OPS_H */
