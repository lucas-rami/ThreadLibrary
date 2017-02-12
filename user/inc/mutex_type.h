/** @file mutex_type.h
 *  @brief This file defines the type for mutex.
 *  @author akanjani, lramire1
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct mutex {
  int init;
  int lock_available;
  int nb_threads_waiting;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
