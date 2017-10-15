#include <lsl_c.h>
#include <stdio.h>

/**
 * This example tests if compiling and linking works
 */

int main(int argc, char* argv[]) {
	printf("Using LSL version %d (%s)\n", lsl_library_version(), lsl_library_info());
}
