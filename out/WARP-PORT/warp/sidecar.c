/*
 * sidecar: check a weft-byte-range/1 sidecar and select the exact bytes it
 * describes (ADDRESSING.md).  Written by Claude (claude.ai/code) for
 * ELSEWHERE, card WARP-PORT, from src.git 75ad5ce4 warp/sidecar.py and the
 * object reads of warp/warp.py.  sidecar(1) is its interface specification
 * and sidecar(5) its format.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sha2.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HEADER_MAX	1024		/* the header and both delimiters */
#define SIDECAR_MAX	1048576		/* the whole sidecar */
#define CONTENT_MAX	67108864	/* this profile's content capacity */
#define VALUE_MAX	64		/* the longest value, a digest */
#define HEX		"0123456789abcdef"
#define DOCID		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" \
			    "0123456789._-"

enum {				/* exit status: the public result */
	S_OK,			/* success */
	S_MALFORMED,		/* description malformed or unsupported */
	S_UNAVAILABLE,		/* sidecar or content unavailable */
	S_MISMATCH,		/* content identity mismatch */
	S_BOUNDS,		/* range out of bounds, or over a limit */
	S_IO,			/* read or write failure */
	S_USAGE			/* usage or setup failure */
};

enum { K_SCHEMA, K_SHA256, K_SIZE, K_OFFSET, K_LENGTH, K_DOCID, K_REV, K_N };
static const char *const keys[K_N] = {	/* writers emit this order */
	"schema", "content_sha256", "content_size", "offset", "length",
	"docid", "revision_sha256"
};

struct desc {				/* one parsed sidecar header */
	char		 val[K_N][VALUE_MAX + 1];
	size_t		 line[K_N];	/* line of each key; 0 when absent */
	uint64_t	 size, offset, length;
};

static char sidecar[SIDECAR_MAX + 1];	/* the sidecar, read once */

static __dead void
usage(void)
{
	fprintf(stderr, "usage: sidecar check file\n"
	    "       sidecar select file content\n");
	exit(S_USAGE);
}

/* Refuse with STATUS, naming PATH, LINE when it is not 0, and the reason. */
static __dead void
refuse(int status, const char *path, size_t line, const char *fmt, ...)
{
	va_list ap;

	if (line > 0)
		fprintf(stderr, "sidecar: %s:%zu: ", path, line);
	else
		fprintf(stderr, "sidecar: %s: ", path);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(status);
}

/*
 * Read from FD into BUF until WANT bytes or end of file; return the count.
 * A read error exits S_IO.
 */
static size_t
readfull(int fd, const char *path, char *buf, size_t want)
{
	size_t got = 0;
	ssize_t n;

	while (got < want) {
		n = read(fd, buf + got, want - got);
		if (n == -1 && errno == EINTR)
			continue;
		if (n == -1)
			err(S_IO, "%s", path);
		if (n == 0)
			break;
		got += n;
	}
	return got;
}

/* Open PATH for reading without blocking on a FIFO; S_UNAVAILABLE if not. */
static int
openfile(const char *path)
{
	int fd;

	if ((fd = open(path, O_RDONLY | O_NONBLOCK)) == -1)
		err(S_UNAVAILABLE, "%s", path);
	return fd;
}

/* Require FD to be a regular file and return its size in *SIZE. */
static void
regular(int fd, const char *path, off_t *size)
{
	struct stat st;

	if (fstat(fd, &st) == -1)
		err(S_IO, "%s", path);
	if (!S_ISREG(st.st_mode))
		refuse(S_UNAVAILABLE, path, 0, "not a regular file");
	*size = st.st_size;
}

/*
 * Parse S as a canonical unsigned decimal, 0 or [1-9][0-9]*, into *V.
 * Overflow is refused digit by digit, before any range arithmetic.
 */
static int
decimal(const char *s, uint64_t *v)
{
	uint64_t n = 0;
	unsigned int d;

	if (s[0] == '\0' || (s[0] == '0' && s[1] != '\0'))
		return 0;
	for (; *s != '\0'; s++) {
		if (*s < '0' || *s > '9')
			return 0;
		d = *s - '0';
		if (n > (UINT64_MAX - d) / 10)
			return 0;
		n = n * 10 + d;
	}
	*v = n;
	return 1;
}

/* Whether S is 64 lowercase hexadecimal digits. */
static int
hex64(const char *s)
{
	return strlen(s) == 64 && strspn(s, HEX) == 64;
}

/*
 * Whether the LEN bytes at S are UTF-8 as RFC 3629 defines it (no overlong
 * form, no surrogate, nothing above U+10FFFF) holding no control character
 * but tab and newline: STANDARDS section 8 refuses control characters in
 * a format, and a body may be shown on a terminal.
 */
static int
textok(const unsigned char *s, size_t len)
{
	size_t i, j, n;
	uint32_t c;

	for (i = 0; i < len; i += n + 1) {
		c = s[i];
		n = 0;
		if (c < 0x80) {
			if ((c < 0x20 && c != '\t' && c != '\n') || c == 0x7f)
				return 0;
			continue;
		}
		/* A lead byte before N more bytes keeps 6 - N value bits. */
		if (c < 0xc2 || c > 0xf4)
			return 0;
		n = c >= 0xf0 ? 3 : c >= 0xe0 ? 2 : 1;
		c &= 0x3f >> n;
		if (len - i - 1 < n)
			return 0;
		for (j = 1; j <= n; j++) {
			if ((s[i + j] & 0xc0) != 0x80)
				return 0;
			c = c << 6 | (s[i + j] & 0x3f);
		}
		if ((n == 2 && c < 0x800) || (n == 3 && c < 0x10000) ||
		    c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff) || c <= 0x9f)
			return 0;
	}
	return 1;
}

/*
 * Take one header entry, exactly key: "value", from P up to the newline at
 * NL.  The value is copied unchecked; check() applies its alphabet.  The
 * value holds no NUL, because parse() refused every NUL in the sidecar.
 */
static void
entry(const char *path, size_t line, const char *p, const char *nl,
    struct desc *d)
{
	const char *colon;
	size_t k, klen, vlen;

	colon = memchr(p, ':', nl - p);
	if (colon == NULL || nl - colon < 4 || colon[1] != ' ' ||
	    colon[2] != '"' || nl[-1] != '"')
		refuse(S_MALFORMED, path, line,
		    "entry is not exactly key: \"value\"");
	klen = colon - p;
	for (k = 0; k < K_N; k++)
		if (strlen(keys[k]) == klen && memcmp(p, keys[k], klen) == 0)
			break;
	if (k == K_N)
		refuse(S_MALFORMED, path, line, "unknown key");
	if (d->line[k] != 0)
		refuse(S_MALFORMED, path, line, "duplicate key %s", keys[k]);
	vlen = nl - 1 - (colon + 3);
	if (vlen > VALUE_MAX)
		refuse(S_MALFORMED, path, line, "%s: value too long", keys[k]);
	memcpy(d->val[k], colon + 3, vlen);
	d->val[k][vlen] = '\0';
	d->line[k] = line;
}

/*
 * Parse the LEN bytes of the sidecar at PATH into D: the header between
 * two exact "---" lines within HEADER_MAX bytes, then the body.
 */
static void
parse(const char *path, size_t len, struct desc *d)
{
	const char *p, *nl, *lim;
	size_t line = 1;

	if (memchr(sidecar, '\0', len) != NULL)
		refuse(S_MALFORMED, path, 0, "NUL byte");
	if (len < 4 || memcmp(sidecar, "---\n", 4) != 0)
		refuse(S_MALFORMED, path, 1, "first line is not ---");
	lim = sidecar + (len < HEADER_MAX ? len : HEADER_MAX);
	for (p = sidecar + 4;; p = nl + 1) {
		line++;
		if ((nl = memchr(p, '\n', lim - p)) == NULL)
			refuse(S_MALFORMED, path, line,
			    "no closing --- within %d bytes", HEADER_MAX);
		if (nl - p == 3 && memcmp(p, "---", 3) == 0)
			break;
		entry(path, line, p, nl, d);
	}
	p = nl + 1;
	if (!textok((const unsigned char *)p, sidecar + len - p))
		refuse(S_MALFORMED, path, 0,
		    "body is not UTF-8 text without control characters");
}

/* Apply each value's alphabet and the key table's presence rules to D. */
static void
check(const char *path, struct desc *d)
{
	const char *docid = d->val[K_DOCID];
	size_t k;

	for (k = K_SCHEMA; k <= K_LENGTH; k++)
		if (d->line[k] == 0)
			refuse(S_MALFORMED, path, 0, "missing key %s", keys[k]);
	if ((d->line[K_DOCID] == 0) != (d->line[K_REV] == 0))
		refuse(S_MALFORMED, path, 0,
		    "docid and revision_sha256 come together or not at all");
	if (strcmp(d->val[K_SCHEMA], "weft-byte-range/1") != 0)
		refuse(S_MALFORMED, path, d->line[K_SCHEMA],
		    "unsupported schema");
	if (!hex64(d->val[K_SHA256]))
		refuse(S_MALFORMED, path, d->line[K_SHA256],
		    "content_sha256 is not 64 lowercase hex digits");
	if (!decimal(d->val[K_SIZE], &d->size))
		refuse(S_MALFORMED, path, d->line[K_SIZE],
		    "content_size is not a canonical decimal");
	if (!decimal(d->val[K_OFFSET], &d->offset))
		refuse(S_MALFORMED, path, d->line[K_OFFSET],
		    "offset is not a canonical decimal");
	if (!decimal(d->val[K_LENGTH], &d->length))
		refuse(S_MALFORMED, path, d->line[K_LENGTH],
		    "length is not a canonical decimal");
	if (d->line[K_DOCID] == 0)
		return;
	if (strlen(docid) > 32 || docid[0] == '\0' || docid[0] == '.' ||
	    strspn(docid, DOCID) != strlen(docid))
		refuse(S_MALFORMED, path, d->line[K_DOCID], "bad docid");
	if (!hex64(d->val[K_REV]) || strspn(d->val[K_REV], "0") == 64)
		refuse(S_MALFORMED, path, d->line[K_REV],
		    "revision_sha256 is not a nonzero lowercase hex digest");
}

/*
 * Serve the range D selects from the content open at CFD.  The whole
 * content is read once into a private buffer, its size and digest are
 * verified there, and the range is written from that same buffer, so a
 * writer changing the file cannot mix identities: a change during the
 * read fails the size or digest check, and one after it is never seen.
 * Nothing reaches stdout before the content is verified.
 */
static void
selectrange(const char *spath, const char *cpath, int cfd, struct desc *d)
{
	char md[SHA256_DIGEST_STRING_LENGTH], extra, *buf;
	size_t n, off;
	ssize_t w;
	off_t fsize;

	if (d->line[K_DOCID] != 0)
		refuse(S_MALFORMED, spath, d->line[K_DOCID],
		    "native provenance is not supported by this profile");
#ifdef SIDECAR_MUTANT
	/* The standing wrong implementation: clamp the range silently. */
	if (d->offset > d->size)
		d->offset = d->size;
	if (d->length > d->size - d->offset)
		d->length = d->size - d->offset;
#endif
	/* Compare before adding: offset + length may exceed UINT64_MAX. */
	if (d->offset > d->size || d->length > d->size - d->offset)
		refuse(S_BOUNDS, spath, 0, "range outside the content");
	if (d->size > CONTENT_MAX)
		refuse(S_BOUNDS, spath, d->line[K_SIZE],
		    "content_size over this profile's capacity of %d bytes",
		    CONTENT_MAX);
	regular(cfd, cpath, &fsize);
	if ((uint64_t)fsize != d->size)
		refuse(S_MISMATCH, cpath, 0, "size differs from content_size");

	/* buf is owned here and freed before the return. */
	if ((buf = malloc(d->size > 0 ? d->size : 1)) == NULL)
		err(S_BOUNDS, "content buffer");
	if (readfull(cfd, cpath, buf, d->size) != d->size ||
	    readfull(cfd, cpath, &extra, 1) != 0)
		refuse(S_MISMATCH, cpath, 0, "size changed while read");
	SHA256Data((const uint8_t *)buf, d->size, md);
	if (strcmp(md, d->val[K_SHA256]) != 0)
		refuse(S_MISMATCH, cpath, 0,
		    "digest differs from content_sha256");

	for (off = d->offset, n = d->length; n > 0; off += w, n -= w) {
		w = write(STDOUT_FILENO, buf + off, n);
		if (w == -1 && errno == EINTR)
			w = 0;
		else if (w == -1)
			err(S_IO, "stdout");
	}
	free(buf);
}

int
main(int argc, char *argv[])
{
	struct desc d;
	size_t len;
	off_t ssize;
	int sfd, cfd = -1;

	if (pledge("stdio rpath", NULL) == -1)
		err(S_USAGE, "pledge");
	if (!(argc == 3 && strcmp(argv[1], "check") == 0) &&
	    !(argc == 4 && strcmp(argv[1], "select") == 0))
		usage();

	/*
	 * Open the operands, then drop every path operation before the first
	 * byte is read; unveil(2) would confine nothing further.
	 */
	sfd = openfile(argv[2]);
	if (argc == 4)
		cfd = openfile(argv[3]);
	if (pledge("stdio", NULL) == -1)
		err(S_USAGE, "pledge");

	regular(sfd, argv[2], &ssize);
	if ((len = readfull(sfd, argv[2], sidecar, SIDECAR_MAX + 1)) >
	    SIDECAR_MAX)
		refuse(S_MALFORMED, argv[2], 0, "over %d bytes", SIDECAR_MAX);
	memset(&d, 0, sizeof(d));
	parse(argv[2], len, &d);
	check(argv[2], &d);
	if (cfd != -1)
		selectrange(argv[2], argv[3], cfd, &d);
	return S_OK;
}
