/* If you want to use assembly language instead of C,
 * delete this autostack.c and provide an autostack.S
 * instead.
 */

#include <thr_internals.h>

void install_autostack(void *stack_high, void *stack_low) {
  task_state.stack_lowest = stack_low;
  task_state.stack_highest = stack_high;
}
