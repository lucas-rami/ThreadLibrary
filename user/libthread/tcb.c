/** @file tcb.c
 *  @brief This file contains the definition for functions related to TCB manipulation
 *  @author akanjani, lramire1
 */

#include <stdlib.h>
#include <global_state.h>

tcb_t* get_tcb() {
  //TODO
  return NULL;
}

/** @brief Find a TCB by its library_tid
 *
 *  @param tcb A thread's TCB
 *  @param tid A library tid
 *
 *  @return 1 if the TCB is the good one (its library_tid is tid), 0 otherwise
 */
int find_tcb(void* tcb, void* tid) {
 tcb_t* t = tcb;
 int* library_tid = tid;
 if (t->library_tid == *library_tid) {
   return 1;
 }
 return 0;
}

/** @brief Hash function for TCBs hash table
 *
 *  @param tcb The TCB to hash
 *  @param nb_buckets The number of buckets in the hash table
 *
 *  @return The TCB's hashed value
 */
unsigned int hash_function_tcb(void* tcb, unsigned int nb_buckets) {
 tcb_t* t = tcb;
 return t->library_tid % nb_buckets;
}
