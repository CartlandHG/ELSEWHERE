/* Red control BANNED: an unbounded copy. */
#include <string.h>

char *
copy(char *d, const char *s)
{
	return strcpy(d, s);
}
