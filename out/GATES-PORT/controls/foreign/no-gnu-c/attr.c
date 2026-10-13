/* Red control NO-GNU-C: __attribute__ in code; this comment does not count. */
static int f(void) __attribute__((unused));
static int f(void) { return 1; }
int main(void) { return 0; }
