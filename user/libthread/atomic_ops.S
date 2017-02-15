/** @file atomic_ops.S
 *  @brief This file contains functions that perform actions atomically
 *  @author akanjani, lramire1
 */

.global atomic_exchange
.global atomic_cmp_xchg

atomic_exchange:
  movl  4(%esp), %edx   // Move &mutex_lock (first argument) into edx
  movl  8(%esp), %eax   // Move val (second argument) into eax
  xchg  (%edx),  %eax   // Exchange *mutex_lock and val atomically
  ret                   // Return from procedure (eax contains the old value)

atomic_cmp_xchg:
  movl    4(%esp), %ecx        // ecx = addr_val (int*)
  movl    8(%esp), %eax        // eax = old_val (int)
  movl    12(%esp),%edx        // edx = new_val (int)
  LOCK cmpxchg (%ecx), %edx    // if (4(%esp) == %eax) 4(%esx) = %edx
  ret                          // Return from procedure
