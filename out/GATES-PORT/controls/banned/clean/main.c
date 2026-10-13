/* Green control: bounded forms, and a longer name that holds a banned one. */
#include <stdio.h>
#include <string.h>

int	xstrcpy(char *, const char *);

int
copy(char *d, const char *s, size_t n)
{
	if (strlcpy(d, s, n) >= n)
		return -1;
	return snprintf(d, n, "%s", s) < 0 ? -1 : xstrcpy(d, s);
}
