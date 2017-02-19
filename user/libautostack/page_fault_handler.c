/** @file page_fault_handler.c
 *
 *  @brief This file contains the implementation of software exception 
 *   handlers for single threaded tasks and multi threaded ones
 *
 *  @author akanjani, lramire1
 */

#include <global_state.h>
#include <syscall.h>
#include <ureg.h>
#include <stddef.h>
#include <simics.h>

/** @brief The exception handler for single threaded tasks
 *
 *  This function handles page faults and attempts to grow the stack as 
 *  appropriate.
 *
 *  @param arg A void pointer which was specified when the handler was 
 *   registered
 *  @param ureg A ureg_t pointer to the ureg area, which will is stored on
 *   the exception stack and contains the register values
 *
 *  @return void
 */
void singlethread_handler(void* arg, ureg_t *ureg) {

  if ( ureg->cause == SWEXN_CAUSE_PAGEFAULT ) {

    // TODO: Add a check for if this is a page fault for heap and not stack
    // TODO: Check if the error flags can help us?

    // Calculate the Number of pages required
    int num_of_pages_reqd =
      ( ( unsigned int )task.stack_lowest / PAGE_SIZE ) - 
      ( ureg->cr2 / PAGE_SIZE );

    int growth_size = num_of_pages_reqd * PAGE_SIZE;

    if ( new_pages( ( (char*)task.stack_lowest - growth_size ),
         growth_size ) < 0 ) {
      // Can't grow the stack further. Something went really wrong
      // Revert back to default behaviour
      vanish();
    }
    task.stack_lowest =  (void *)( (char*)task.stack_lowest - growth_size );
  }

  // Register the handler again
  if ( swexn( exception_handler_stack + PAGE_SIZE,
       singlethread_handler, NULL, ureg ) < 0 ) {
    // Can't register the handler
    // TODO: assert?
  }
}

/** @brief Exception handler for multithreaded application
 *
 *  The function simply vanish the current task.
 *
 *  @param arg  Optional argument for exception handler
 *  @param ureg Structure holding information about the exception's cause
 *   and the values of some registers
 * 
 *  @return void
 */
 void multithread_handler(void* arg, ureg_t *ureg) {
   vanish();
 }

