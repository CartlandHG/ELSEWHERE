/*
 * route: the queue and dispatcher of the spine (SPINE.md, goal SPINE-3).
 * Written by Claude (claude.ai/code), started by Claudia for Hugh.
 * route(8) is its interface specification.
 */

#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sha2.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef ROUTE_TEST
#include <signal.h>
#endif

#define BLOCK		512		/* a ustar block */
#define CARD_MAX	65536		/* a card, and route.conf */
#define HEADER_MAX	1024		/* a header; every value is shorter */
#define JOB_MAX		64
#define SEAT_MAX	32
#define OUT_MAX		16		/* names in outputs */
#define REQ_MAX		128		/* a request line, with its newline */
#define RETURN_HARD	67108864UL	/* the largest return_max */
#define UPPER		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER		"abcdefghijklmnopqrstuvwxyz"
#define DIGITS		"0123456789"
#define validjob(s)	inalpha((s), UPPER DIGITS, UPPER DIGITS "-", JOB_MAX)
#define validseat(s)	inalpha((s), LOWER, LOWER DIGITS "-", SEAT_MAX)

enum { RS_DONE, RS_REFUSED, RS_ERROR, RS_NONE };	/* exit status */
enum { QUEUE, SENT, DELIVERED, RETURNED, HELD, TMP };	/* directories */
static const char *const names[] = {
	"queue", "sent", "delivered", "returned", "held", "tmp"
};

struct want {				/* a name the return must hold */
	const char	*name;
	char		 sum[SHA256_DIGEST_STRING_LENGTH];
	int		 seen;		/* archive entries of this name */
	int		 listed;	/* SHA256SUMS lines of this name */
};

static const char *sdir = "/var/route";
static const char *confpath = "/etc/route.conf";
static const char *cardpath = "/usr/local/bin/card";
static int lockfd = -1, ledgerfd = -1;
static char card[CARD_MAX + 1];		/* the card in hand */
static size_t cardlen;

static __dead void
usage(void)
{
	errx(RS_ERROR, "usage: route [-d dir] add file | hold job | "
	    "release job | status | serve seat");
}

/* Format a path into BUF, which holds PATH_MAX bytes; never truncate. */
static void
mkpath(char *buf, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(buf, PATH_MAX, fmt, ap);
	va_end(ap);
	if (n < 0 || n >= PATH_MAX)
		errx(RS_ERROR, "path too long");
}

/* Whether S is one byte of FIRST, then bytes of REST, at most MAX. */
static int
inalpha(const char *s, const char *first, const char *rest, size_t max)
{
	size_t len = strlen(s);

	return len > 0 && len <= max && strchr(first, s[0]) != NULL &&
	    strspn(s + 1, rest) == len - 1;
}

/* Parse a canonical decimal of at most MAX; 8 digits cannot overflow. */
static int
parsedec(const char *s, unsigned long max, unsigned long *v)
{
	size_t len = strlen(s);

	if (len == 0 || len > 8 || strspn(s, DIGITS) != len ||
	    (s[0] == '0' && len > 1))
		return 0;
	*v = strtoul(s, NULL, 10);
	return *v <= max;
}

/*
 * Copy into VAL (VSZ bytes) the value of KEY from the header of BUF: the
 * first line between the opening "---" line and the next "---" line that
 * begins with the key, a colon, a space and a double quote.  card(1) has
 * made each header key one such line, and a body line is never reached.
 */
static int
hval(const char *buf, size_t len, const char *key, char *val, size_t vsz)
{
	const char *p, *nl, *end = buf + len;
	size_t n, k = strlen(key);

	if (len < 4 || memcmp(buf, "---\n", 4) != 0)
		return -1;
	for (p = buf + 4; (nl = memchr(p, '\n', end - p)) != NULL;
	    p = nl + 1) {
		if ((n = nl - p) == 3 && memcmp(p, "---", 3) == 0)
			return -1;
		if (n < k + 5 || memcmp(p, key, k) != 0 ||
		    memcmp(p + k, ": \"", 3) != 0)
			continue;
		if ((n -= k + 4) >= vsz || p[k + 3 + n] != '"')
			return -1;
		memcpy(val, p + k + 3, n);
		val[n] = '\0';
		return strcspn(val, "\"") == n ? 0 : -1;
	}
	return -1;
}

/* Split outputs S in place at single spaces into at most OUT_MAX names. */
static int
splitnames(char *s, char **v)
{
	char *p;
	int n = 0;

	while ((p = strsep(&s, " ")) != NULL) {
		if (*p == '\0' || n == OUT_MAX)
			return -1;
		v[n++] = p;
	}
	return n;
}

/* Read PATH into BUF (MAX + 1 bytes): 0, 1 if longer than MAX, or -1. */
static int
readfile(const char *path, char *buf, size_t max, size_t *len)
{
	ssize_t r = 0;
	int fd;

	if ((fd = open(path, O_RDONLY | O_CLOEXEC)) == -1)
		return -1;
	for (*len = 0; *len <= max; *len += r)
		if ((r = read(fd, buf + *len, max + 1 - *len)) <= 0)
			break;
	(void)close(fd);
	return r == -1 ? -1 : *len > max;
}

static void
syncdir(const char *path)
{
	int fd;

	if ((fd = open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC)) == -1 ||
	    fsync(fd) == -1)
		err(RS_ERROR, "fsync %s", path);
	(void)close(fd);
}

/* Create or truncate DIR/NAME, write LEN bytes, fsync it and DIR. */
static void
writefile(const char *dir, const char *name, const void *buf, size_t len)
{
	char path[PATH_MAX];
	const char *p = buf;
	ssize_t n = 0;
	int fd;

	mkpath(path, "%s/%s", dir, name);
	if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
	    0640)) == -1)
		err(RS_ERROR, "%s", path);
	for (; len > 0; p += n, len -= n)
		if ((n = write(fd, p, len)) == -1)
			err(RS_ERROR, "%s", path);
	if (fsync(fd) == -1 || close(fd) == -1)
		err(RS_ERROR, "%s", path);
	syncdir(dir);
}

/* Return the stage that holds JOB, or -1 if none does. */
static int
jobstage(const char *job)
{
	char p[PATH_MAX];
	struct stat st;
	int i;

	for (i = QUEUE; i < TMP; i++) {
		mkpath(p, "%s/%s/%s", sdir, names[i], job);
		if (stat(p, &st) == 0)
			return i;
		if (errno != ENOENT)
			err(RS_ERROR, "%s", p);
	}
	return -1;
}

/* Load the card of JOB in STAGE into card[], and its seat into SEAT. */
static void
loadcard(int stage, const char *job, char *seat)
{
	char p[PATH_MAX];

	mkpath(p, "%s/%s/%s/card", sdir, names[stage], job);
	if (readfile(p, card, CARD_MAX, &cardlen) != 0 ||
	    hval(card, cardlen, "seat", seat, SEAT_MAX + 1) == -1 ||
	    !validseat(seat))
		errx(RS_ERROR, "%s: unreadable, or has no valid seat", p);
}

/* Load the card of SEAT's JOB and return its stage; refuse any other. */
static int
seatjob(const char *seat, const char *job)
{
	char s[SEAT_MAX + 1] = "";
	int st;

	if ((st = jobstage(job)) != -1)
		loadcard(st, job, s);
	if (st == -1 || strcmp(s, seat) != 0)
		errx(RS_REFUSED, "%s: no such job for this seat", job);
	return st;
}

static int
notdot(const struct dirent *d)
{
	return strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0;
}

/* List STAGE in byte order (alphasort(3) is strcmp(3)); exit frees it. */
static int
listjobs(int stage, struct dirent ***list)
{
	char p[PATH_MAX];
	int i, n;

	mkpath(p, "%s/%s", sdir, names[stage]);
	if ((n = scandir(p, list, notdot, alphasort)) == -1)
		err(RS_ERROR, "%s", p);
	for (i = 0; i < n; i++)
		if (!validjob((*list)[i]->d_name))
			errx(RS_ERROR, "%s: holds a non-job entry", p);
	return n;
}

/*
 * Commit JOB from FROM to TO with one rename(2) of its directory (of SRC,
 * if given), then append one ledger line with one write(2).  What was
 * staged is already fsynced.  Ids are unique and job directories never
 * empty, so the rename never replaces a job.  A kill between the two
 * leaves consistent state and the ledger one line short.
 */
static void
commit(const char *verb, const char *job, const char *src, int from, int to,
    const char *sum)
{
	char p[PATH_MAX], q[PATH_MAX], line[256];
	int n;

	mkpath(p, "%s/%s/%s", sdir, names[from], job);
	mkpath(q, "%s/%s/%s", sdir, names[to], job);
	if (rename(src ? src : p, q) == -1)
		err(RS_ERROR, "rename %s", job);
	mkpath(p, "%s/%s", sdir, names[to]);
	syncdir(p);
	mkpath(p, "%s/%s", sdir, names[from]);
	syncdir(p);
#ifdef ROUTE_TEST
	if (getenv("ROUTE_FAULT") && strcmp(getenv("ROUTE_FAULT"), verb) == 0)
		raise(SIGKILL);
#endif
	n = snprintf(line, sizeof(line), "%lld %s %s %s %s%s%s\n",
	    (long long)time(NULL), verb, job, names[from], names[to],
	    sum ? " " : "", sum ? sum : "");
	if (n < 0 || (size_t)n >= sizeof(line))
		errx(RS_ERROR, "ledger line too long");
	if (write(ledgerfd, line, n) != n || fsync(ledgerfd) == -1)
		err(RS_ERROR, "ledger, after committing %s", job);
}

/* add FILE: stage the card and route.conf, each read once; run card(1). */
static void
cmd_add(const char *file)
{
	char stg[PATH_MAX], p[PATH_MAX], cf[PATH_MAX], conf[CARD_MAX + 1];
	char job[JOB_MAX + 1], outs[HEADER_MAX], rmax[16], *v[OUT_MAX];
	char *argv[] = { "card", "-c", cf, p, NULL }, *envp[] = { NULL };
	unsigned long max;
	size_t i, len, conflen;
	pid_t pid;
	int k, n = 0, st;

	if ((st = readfile(file, card, CARD_MAX, &cardlen)) == -1)
		err(RS_ERROR, "%s", file);
	if (st == 1)
		errx(RS_REFUSED, "card over %d bytes", CARD_MAX);
	if (readfile(confpath, conf, CARD_MAX, &conflen) != 0)
		errx(RS_ERROR, "%s: unreadable or too long", confpath);
	mkpath(stg, "%s/tmp/add.XXXXXXXXXX", sdir);
	if (mkdtemp(stg) == NULL)
		err(RS_ERROR, "%s", stg);
	writefile(stg, "card", card, cardlen);
	writefile(stg, "conf", conf, conflen);
	mkpath(p, "%s/card", stg);
	mkpath(cf, "%s/conf", stg);

	/* card(1) gets an empty environment and descriptors 0 to 2 only. */
	if ((pid = fork()) == -1)
		err(RS_ERROR, "fork");
	if (pid == 0) {
		execve(cardpath, argv, envp);
		_exit(127);
	}
	if (waitpid(pid, &st, 0) == -1)
		err(RS_ERROR, "waitpid");
	if (!WIFEXITED(st) || WEXITSTATUS(st) == 127)
		errx(RS_ERROR, "%s did not run to completion", cardpath);
	if (WEXITSTATUS(st) != 0)
		errx(RS_REFUSED, "card refused by card(1)");
	if (unlink(cf) == -1)
		err(RS_ERROR, "%s", cf);

	/* Values come only from the headers of the bytes card(1) judged. */
	if (hval(card, cardlen, "job", job, sizeof(job)) == -1 ||
	    !validjob(job) ||
	    hval(card, cardlen, "outputs", outs, sizeof(outs)) == -1 ||
	    (n = splitnames(outs, v)) == -1 ||
	    hval(conf, conflen, "return_max", rmax, sizeof(rmax)) == -1 ||
	    !parsedec(rmax, RETURN_HARD, &max) || max < 1024)
		errx(RS_ERROR, "card(1) accepted values route cannot read");
	/* ustar holds a name whole, or split at one slash into 155 and 100. */
	for (k = 0; k < n; k++) {
		len = strlen(v[k]);
		for (i = 0; len > 100 && i < len && i <= 155; i++)
			if (v[k][i] == '/' && len - i - 1 <= 100)
				break;
		if (len > 100 && (i == len || i > 155))
			errx(RS_REFUSED, "%s: ustar cannot hold output %s",
			    job, v[k]);
	}
	if (jobstage(job) != -1)
		errx(RS_REFUSED, "%s: exists; an identity is never reused",
		    job);

	/* The job keeps the return_max in force when it was added. */
	strlcat(rmax, "\n", sizeof(rmax));
	writefile(stg, "return_max", rmax, strlen(rmax));
	commit("add", job, stg, TMP, QUEUE, NULL);
}

/* hold JOB: from queue/, sent/ or delivered/, recording which. */
static void
cmd_hold(const char *job)
{
	char p[PATH_MAX], line[16];
	int s;

	if ((s = jobstage(job)) == -1 || s == RETURNED || s == HELD)
		errx(RS_REFUSED, "%s: not in queue, sent or delivered", job);
	mkpath(p, "%s/%s/%s", sdir, names[s], job);
	(void)snprintf(line, sizeof(line), "%s\n", names[s]);
	writefile(p, "held_from", line, strlen(line));
	commit("hold", job, NULL, s, HELD, NULL);
}

/* release JOB: only if held from queue/; one that left it is not resent. */
static void
cmd_release(const char *job)
{
	char p[PATH_MAX], buf[16];
	size_t len;

	if (jobstage(job) != HELD)
		errx(RS_REFUSED, "%s: not held", job);
	mkpath(p, "%s/held/%s/held_from", sdir, job);
	if (readfile(p, buf, sizeof(buf) - 1, &len) != 0)
		errx(RS_ERROR, "%s: unreadable", p);
	if (len != 6 || memcmp(buf, "queue\n", 6) != 0)
		errx(RS_REFUSED, "%s: held after leaving the queue; a retry "
		    "is a new card", job);
	commit("release", job, NULL, HELD, QUEUE, NULL);
}

/* status: "STAGE JOB SEAT" lines in byte order, so stages A to Z. */
static void
cmd_status(void)
{
	static const int order[] = { DELIVERED, HELD, QUEUE, RETURNED, SENT };
	char seat[SEAT_MAX + 1];
	struct dirent **d;
	int i, j, n;

	for (i = 0; i < 5; i++) {
		n = listjobs(order[i], &d);
		for (j = 0; j < n; j++) {
			loadcard(order[i], d[j]->d_name, seat);
			printf("%s %s %s\n", names[order[i]], d[j]->d_name,
			    seat);
		}
	}
	if (fflush(stdout) == EOF || ferror(stdout))
		err(RS_ERROR, "stdout");
}

/* next: commit SEAT's first queued job to sent/, then send it, once. */
static void
serve_next(const char *seat)
{
	char s[SEAT_MAX + 1], sum[SHA256_DIGEST_STRING_LENGTH];
	struct dirent **d;
	int i, n;

	n = listjobs(QUEUE, &d);
	for (i = 0; i < n; i++) {
		loadcard(QUEUE, d[i]->d_name, s);
		if (strcmp(s, seat) == 0)
			break;		/* card[] holds this job's card */
	}
	if (i == n)
		exit(RS_NONE);
	SHA256Data((const u_int8_t *)card, cardlen, sum);
	commit("next", d[i]->d_name, NULL, QUEUE, SENT, NULL);
	if (printf("card %s %s %zu\n", d[i]->d_name, sum, cardlen) < 0 ||
	    fwrite(card, 1, cardlen, stdout) != cardlen ||
	    fflush(stdout) == EOF)
		err(RS_ERROR, "stdout");
}

/*
 * Parse a numeric ustar field of LEN (at most 12) bytes: octal digits,
 * then only NULs and spaces.  Twelve digits stay below 2^36.
 */
static int
octal(const u_int8_t *p, size_t len, unsigned long long *v)
{
	size_t i;

	for (i = 0, *v = 0; i < len && p[i] >= '0' && p[i] <= '7'; i++)
		*v = *v * 8 + (p[i] - '0');
	if (i == 0)
		return 0;
	for (; i < len; i++)
		if (p[i] != ' ' && p[i] != '\0')
			return 0;
	return 1;
}

static int
iszero(const u_int8_t *p, size_t len)
{
	while (len > 0)
		if (p[--len] != 0)
			return 0;
	return 1;
}

/*
 * Check archive A of N bytes against the card's outputs OUTS (split in
 * place), as route(8) describes.  Return NULL if it holds, else a reason.
 */
static const char *
checktar(const u_int8_t *a, size_t n, char *outs)
{
	struct want w[OUT_MAX + 1];
	char name[155 + 1 + 100 + 1], *v[OUT_MAX];
	const u_int8_t *h, *s = NULL, *nl;
	unsigned long long ck, hck, size;
	size_t i, off, pl, len = 0;
	int k, nw, lines = 0;

	memset(w, 0, sizeof(w));
	if ((nw = splitnames(outs, v)) == -1)
		return "the card's outputs are unreadable";
	for (k = 0; k < nw; k++)
		w[k].name = v[k];
	w[nw++].name = "SHA256SUMS";
	if (n % BLOCK != 0)
		return "the size is not a multiple of 512";
	/*
	 * OFF stays a multiple of BLOCK, at most N: an entry's size is checked
	 * against the whole blocks after its header before OFF moves past it.
	 */
	for (off = 0; n - off >= BLOCK && !iszero(a + off, BLOCK); ) {
		h = a + off;
		for (i = 0, ck = 0; i < BLOCK; i++)
			ck += (i >= 148 && i < 156) ? ' ' : h[i];
		if (!octal(h + 148, 8, &hck) || hck != ck)
			return "a header checksum is wrong";
		if (memcmp(h + 257, "ustar\0" "00", 8) != 0)
			return "a header is not ustar";
		if (h[156] != '0' && h[156] != '\0')
			return "an entry is not a regular file";
		if (!octal(h + 124, 12, &size) || size > n - off - BLOCK)
			return "an entry runs past the end of the archive";
		pl = strnlen((const char *)h + 345, 155);
		(void)snprintf(name, sizeof(name), "%.*s%s%.*s", (int)pl,
		    (const char *)h + 345, pl > 0 ? "/" : "",
		    (int)strnlen((const char *)h, 100), (const char *)h);
		for (k = 0; k < nw && strcmp(w[k].name, name) != 0; k++)
			continue;
		if (k == nw || w[k].seen++)
			return "an entry is not in outputs, or is repeated";
		if (k == nw - 1) {
			s = h + BLOCK;
			len = size;
		} else
			SHA256Data(h + BLOCK, size, w[k].sum);
		off += BLOCK + (size + BLOCK - 1) / BLOCK * BLOCK;
	}
	if (n - off < 2 * BLOCK || !iszero(a + off, n - off))
		return "under two zero blocks at the end, or data after them";
	for (k = 0; k < nw; k++)
		if (!w[k].seen)
			return "an output is missing";

	/* SHA256SUMS lists each output once, as "HEX  NAME", never itself. */
	for (nw--; len > 0; s = nl + 1, len -= i + 1) {
		if ((nl = memchr(s, '\n', len)) == NULL)
			return "SHA256SUMS: a line has no newline";
		if ((i = nl - s) < 67 || s[64] != ' ' || s[65] != ' ' ||
		    strspn((const char *)s, DIGITS "abcdef") != 64)
			return "SHA256SUMS: a line is malformed";
		for (k = 0; k < nw; k++)
			if (strlen(w[k].name) == i - 66 &&
			    memcmp(w[k].name, s + 66, i - 66) == 0)
				break;
		if (k == nw || w[k].listed++)
			return "SHA256SUMS: a line names no output, or repeats";
		if (memcmp(w[k].sum, s, 64) != 0)
			return "SHA256SUMS: a digest does not match its file";
		lines++;
	}
	return lines == nw ? NULL : "SHA256SUMS: an output is not listed";
}

/*
 * returned JOB SIZE: read exactly SIZE bytes, check them, and commit
 * delivered/ to returned/ with the archive inside.  Before the lock this
 * reads only what never changes once a job is added (card, return_max),
 * so a slow upload holds up no other channel; the stage is checked again
 * under the lock.  The archive is owned here, from malloc(3) to free(3).
 */
static void
serve_returned(const char *seat, const char *job, unsigned long size)
{
	char p[PATH_MAX], outs[HEADER_MAX], buf[16];
	char sum[SHA256_DIGEST_STRING_LENGTH], old[SHA256_DIGEST_STRING_LENGTH];
	const char *why;
	unsigned long max;
	u_int8_t *a;
	size_t got;
	ssize_t r = 0;
	int st;

	if ((st = seatjob(seat, job)) != DELIVERED && st != RETURNED)
		errx(RS_REFUSED, "%s: not delivered", job);
	mkpath(p, "%s/%s/%s/return_max", sdir, names[st], job);
	if (readfile(p, buf, sizeof(buf) - 1, &got) != 0 || got < 2 ||
	    buf[got - 1] != '\n')
		errx(RS_ERROR, "%s: unreadable", p);
	buf[got - 1] = '\0';
	if (!parsedec(buf, RETURN_HARD, &max) ||
	    hval(card, cardlen, "outputs", outs, sizeof(outs)) == -1)
		errx(RS_ERROR, "%s: return_max or outputs unreadable", job);
	if (size > max)
		errx(RS_REFUSED, "%s: %lu bytes is over return_max", job, size);

	if ((a = malloc(size > 0 ? size : 1)) == NULL)
		err(RS_ERROR, "malloc");
	for (got = 0; got < size; got += r)
		if ((r = read(STDIN_FILENO, a + got, size - got)) == -1)
			err(RS_ERROR, "stdin");
		else if (r == 0)
			errx(RS_REFUSED, "%s: archive truncated", job);
	if ((why = checktar(a, size, outs)) != NULL)
		errx(RS_REFUSED, "%s: %s", job, why);
	SHA256Data(a, size, sum);

	if (flock(lockfd, LOCK_EX) == -1)
		err(RS_ERROR, "lock");
	if ((st = jobstage(job)) == RETURNED) {
		mkpath(p, "%s/returned/%s/return.tar", sdir, job);
		if (SHA256File(p, old) == NULL)
			err(RS_ERROR, "%s", p);
		if (strcmp(old, sum) != 0)
			errx(RS_REFUSED, "%s: returned with other bytes", job);
	} else if (st != DELIVERED)
		errx(RS_REFUSED, "%s: no longer delivered", job);
	else {
		mkpath(p, "%s/delivered/%s", sdir, job);
		writefile(p, "return.tar", a, size);
		commit("returned", job, NULL, DELIVERED, RETURNED, sum);
	}
	free(a);
}

/* serve SEAT: one request line, read a byte at a time to stop at '\n'. */
static void
cmd_serve(const char *seat)
{
	char line[REQ_MAX], *tok[4], *p;
	unsigned long size;
	size_t len = 0;
	ssize_t r;
	int n, st;

	do {
		if (len == REQ_MAX)
			errx(RS_REFUSED, "request over %d bytes", REQ_MAX);
		if ((r = read(STDIN_FILENO, line + len, 1)) == -1)
			err(RS_ERROR, "stdin");
		if (r == 0)
			errx(RS_REFUSED, "request without a newline");
	} while (line[len++] != '\n');
	line[len - 1] = '\0';
	n = memchr(line, '\0', len - 1) != NULL ? 4 : 0;
	for (p = line; n < 4 && (tok[n] = strsep(&p, " ")) != NULL; n++)
		if (*tok[n] == '\0')
			n = 4;

	if (n == 3 && strcmp(tok[0], "returned") == 0 && validjob(tok[1]) &&
	    parsedec(tok[2], RETURN_HARD, &size))
		serve_returned(seat, tok[1], size);	/* it locks later */
	else if (flock(lockfd, LOCK_EX) == -1)
		err(RS_ERROR, "lock");
	else if (n == 1 && strcmp(tok[0], "next") == 0)
		serve_next(seat);
	else if (n != 2 || strcmp(tok[0], "delivered") != 0 ||
	    !validjob(tok[1]))
		errx(RS_REFUSED, "malformed request");
	else if ((st = seatjob(seat, tok[1])) == SENT)
		commit("delivered", tok[1], NULL, SENT, DELIVERED, NULL);
	else if (st != DELIVERED && st != RETURNED)	/* else a repeat */
		errx(RS_REFUSED, "%s: not sent", tok[1]);
}

/*
 * Reduce authority before any untrusted byte is read: the state directory
 * (read-only for status); for add, its FILE, route.conf and card(1).
 */
static void
confine(const char *verb, const char *file)
{
	int add = strcmp(verb, "add") == 0, ro = strcmp(verb, "status") == 0;

	if (unveil(sdir, ro ? "r" : "rwc") == -1 || (add &&
	    (unveil(file, "r") == -1 || unveil(confpath, "r") == -1 ||
	    unveil(cardpath, "x") == -1)) || unveil(NULL, NULL) == -1)
		err(RS_ERROR, "unveil");
	if (pledge(add ? "stdio rpath wpath cpath flock proc exec" :
	    ro ? "stdio rpath flock" : "stdio rpath wpath cpath flock",
	    NULL) == -1)
		err(RS_ERROR, "pledge");
}

/* Check the layout, which only the install creates, and open its files. */
static void
openstate(int ro)
{
	char p[PATH_MAX];
	struct stat st;
	int i;

	for (i = QUEUE; i <= TMP; i++) {
		mkpath(p, "%s/%s", sdir, names[i]);
		if (stat(p, &st) == -1 || !S_ISDIR(st.st_mode))
			errx(RS_ERROR, "%s: missing from the state layout", p);
	}
	mkpath(p, "%s/lock", sdir);
	if ((lockfd = open(p, O_RDONLY | O_CLOEXEC)) == -1)
		err(RS_ERROR, "%s", p);
	mkpath(p, "%s/ledger", sdir);
	if ((ledgerfd = open(p, O_CLOEXEC |
	    (ro ? O_RDONLY : O_WRONLY | O_APPEND))) == -1)
		err(RS_ERROR, "%s", p);
}

int
main(int argc, char *argv[])
{
	const char *verb, *arg;
	int ch;

	setprogname("route");		/* the same prefix in every build */
#ifdef ROUTE_TEST
	if (getenv("ROUTE_CONF") != NULL)
		confpath = getenv("ROUTE_CONF");
	if (getenv("ROUTE_CARD") != NULL)
		cardpath = getenv("ROUTE_CARD");
#endif
	while ((ch = getopt(argc, argv, "d:")) != -1) {
		if (ch != 'd')
			usage();
		sdir = optarg;
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage();
	verb = argv[0];
	arg = argv[1];			/* NULL for status */
	if (strcmp(verb, "status") == 0 ? argc != 1 : argc != 2 ||
	    (strcmp(verb, "add") != 0 && strcmp(verb, "hold") != 0 &&
	    strcmp(verb, "release") != 0 && strcmp(verb, "serve") != 0))
		usage();
	/* Operands are checked before any path is built from them. */
	if ((strcmp(verb, "hold") == 0 || strcmp(verb, "release") == 0) &&
	    !validjob(arg))
		errx(RS_REFUSED, "not a job id");
	if (strcmp(verb, "serve") == 0 && !validseat(arg))
		errx(RS_ERROR, "not a seat name");

	confine(verb, arg);
	openstate(strcmp(verb, "status") == 0);
	if (strcmp(verb, "serve") == 0)
		cmd_serve(arg);		/* it takes the lock itself */
	else if (flock(lockfd, LOCK_EX) == -1)
		err(RS_ERROR, "lock");
	else if (strcmp(verb, "add") == 0)
		cmd_add(arg);
	else if (strcmp(verb, "hold") == 0)
		cmd_hold(arg);
	else if (strcmp(verb, "release") == 0)
		cmd_release(arg);
	else
		cmd_status();
	return RS_DONE;
}
