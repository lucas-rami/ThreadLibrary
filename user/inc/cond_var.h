/** @file cond_var.h
 *  @brief This file declares the functions to use condition variables.
 *  @author akanjani, lramire1
 */

#ifndef _COND_VAR_H_
#define _COND_VAR_H_

#include <cond_type.h>
#include <mutex.h>

#define CVAR_INITIALIZED 1
#define CVAR_UNINITIALIZED 0

int cvar_init( cond_t *cond_var );
void cvar_wait( cond_t *cond_var, mutex_t *lock );
void cvar_signal( cond_t *cond_var );
void cvar_broadcast( cond_t *cond_var );

#endif /* _COND_VAR_H_ */
