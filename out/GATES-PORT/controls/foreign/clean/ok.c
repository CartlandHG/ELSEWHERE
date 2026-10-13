/* Green control: naming __linux__ or __attribute__ in a comment is not code. */
#include <stdio.h>
#include <unistd.h>

int
main(void)
{
	if (pledge("stdio", NULL) == -1)
		return 1;
	puts("ok");	// a C99 comment naming __GLIBC__
	return 0;
}
