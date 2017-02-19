/* If you want to use assembly language instead of C,
 * delete this autostack.c and provide an autostack.S
 * instead.
 */

#include <global_state.h>
#include <syscall.h>
#include <page_fault_handler.h>
#include <stddef.h>
#include <simics.h> 
#include <stdlib.h>

void install_autostack( void *stack_high, void *stack_low ) {
  task_state.stack_lowest = stack_low;
  task_state.stack_highest = stack_high;
  // Registerk_state.a handler using swexn
  lprintf( "autostack called" );

  // exception_handler_stack = ( char* ) malloc( sizeof(char) * PAGE_SIZE );
  lprintf( "The excpetion handler stack starts at address %p\n", exception_handler_stack );

  if ( 
    swexn( exception_handler_stack + PAGE_SIZE, singlethread_handler, NULL, NULL ) < 0 ) {
    lprintf( "registration of exception handler failed\n" );
    // Can't register the handler
    // TODO: assert?
  }
  lprintf( "handler should be registered" );
}
