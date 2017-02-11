/** @file print_test.c
 *
 *  @brief Test to verify stubs are working for vanish, set_status and print
 *
 *  @author akanjani, lramire1
 */

#include <syscall.h>
#include <simics.h>

int main()
{
	char *s = "Yay! The print works\n";

	// Verify the above string gets printed on console
	print( 21, s );

	// Verify the process exits with 20
	return 20;
}
