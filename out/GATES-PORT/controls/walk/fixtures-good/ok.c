#include <string.h>

size_t
copy(char *d, const char *s, size_t n)
{
	return strlcpy(d, s, n);
}
