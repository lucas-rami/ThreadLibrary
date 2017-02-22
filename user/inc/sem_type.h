/** @file sem_type.h
 *  @brief This file defines the type for semaphores.
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H

#include <cond_type.h>
#include <mutex_type.h>

typedef struct sem {
  /* fill this in */
  int init, wokenup_waiting, available_resources;
  cond_t cvar;
  mutex_t lock;
} sem_t;

#endif /* _SEM_TYPE_H */
