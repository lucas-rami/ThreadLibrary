#include <global_state.h>
#include <syscall.h>
#include <ureg.h>
#include <stddef.h>
#include <simics.h>

void singlethread_handler(void* arg, ureg_t *ureg) {
  // MAGIC_BREAK;

  lprintf( "Exception handler called\n" );


  // MAGIC_BREAK;

  if ( ureg->cause == SWEXN_CAUSE_PAGEFAULT ) {

    // TODO: Add a check for if this is a page fault for heap and not stack
    // TODO: Check if the error flags can help us?

    // Calculate the Number of pages required
    int num_of_pages_reqd =
      ( ( unsigned int )task_state.stack_lowest / PAGE_SIZE ) - 
      ( ureg->cr2 / PAGE_SIZE );

    lprintf( "The stack lowest is %p and the fault address is %p\n", task_state.stack_lowest, (void *)ureg->cr2 );
    lprintf( "The number of pages reqd is %d\n", num_of_pages_reqd );


    MAGIC_BREAK;
    if ( new_pages( task_state.stack_lowest, num_of_pages_reqd * PAGE_SIZE ) < 0 ) {

      lprintf( "New pages failed\n" );
      // Can't grow the stack further. Something went really wrong
      // Revert back to default behaviour
      vanish();
    }
  }
  lprintf( "Returning from exception handler\n" );
  MAGIC_BREAK;

  // Register the handler again
  if (
    swexn( exception_handler_stack + PAGE_SIZE, singlethread_handler, NULL, ureg ) < 0 ) {
    // Can't register the handler
    // TODO: assert?

    lprintf( "Exception handler couldnt be re installed\n" );
  }
}
