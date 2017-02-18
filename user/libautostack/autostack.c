/* If you want to use assembly language instead of C,
 * delete this autostack.c and provide an autostack.S
 * instead.
 */

#include <global_state.h>

void install_autostack( void *stack_high, void *stack_low ) {
  task_state.stack_lowest = stack_low;
  task_state.stack_highest = stack_high;
  // Register a handler using swexn
  /*if ( 
    swexn( exception_handler_stack, singlethread_handler, NULL, NULL ) < 0 ) {
    // Can't register the handler
    // TODO: assert?
  }*/
}

/*void singlethread_handler( void *arg, ureg_t *ureg ) {
  // Find the page aligned boundaries and then allocate whatever number of 
  // pages we have to
  // Also decide for a sta
}*/
