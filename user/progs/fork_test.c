/** @file fork_test.c
 *
 *  @brief Test to verify stubs are working for fork, wait, sleep, gettid,
 *   get_cursor_pos, get_ticks, getchar and set_cursor_pos
 *
 *  @author akanjani, lramire1
 */

#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <simics.h>

int main()
{
	// Fork a child
	int pid = fork();
	if ( pid < 0 ) {
		// Fork failed
		printf( "fork failed\n" );
		return -1;

	} else if ( pid == 0 ) {
		// Child process

		// Verify the child sleeps
		sleep(10000);

		// Verify that the tread ID is valid
		printf( "Child process. Thread id is %d\n", gettid() );
		int row, col;
		get_cursor_pos( &row, &col );

		// Verify the row and pos is on console
		printf( "row %d and col %d\n", row, col );

		// See how many ticks have passed
		printf( "get ticks returns %u\n", get_ticks() );

		// Read a character from the console and echo it back
		printf( "Enter a character\n" );
		char c = getchar();
		printf( "You entered %c\n", c );
		
		// Change the cursor position
		printf( "Changing cursor to two lines after current line\n" );
		get_cursor_pos( &row, &col );
		set_cursor_pos( row + 2, 0 );
		printf( "Two lines later...\n" );

		exit(0);
	}

	// Wait for the child to finish
	int status;
	if ( wait( &status ) < 0 ) {
		// Wait failed
		printf( "wait failed\n" );
		return -1;
	}

	// If wait works, this should be printed after child process exits
	printf( "Should print after child\n" );

	return 0;
}
