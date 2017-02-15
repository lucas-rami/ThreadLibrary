/** @file spinlock.h
 *  @brief This file declares the functions to use a spinlock
 *  @author akanjani, lramire1
 */

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

typedef int spinlock_t;

int spinlock_init( spinlock_t *lock );
void spinlock_acquire( spinlock_t *lock );
void spinlock_release( spinlock_t *lock );

#endif /* _SPINLOCK_H_ */
