/*
 * Copyright (c) 2017-2026  Joachim Wiberg <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_FUNOPEN
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct ops {
	char   *buf;
	size_t  len;		/* capacity of buf                    */
	size_t  size;		/* bytes of valid data, limits reads  */
	size_t  pos;		/* current read/write position        */
};

typedef struct ops ops_t;

static int readfn(void *arg, char *buf, int len)
{
	ops_t *ops = (ops_t *)arg;
	size_t avail = ops->size > ops->pos ? ops->size - ops->pos : 0;

	if ((size_t)len > avail)
		len = (int)avail;

	memcpy(buf, &ops->buf[ops->pos], len);
	ops->pos += len;

	return len;
}

static int writefn(void *arg, const char *buf, int len)
{
	ops_t *ops = (ops_t *)arg;
	size_t space = ops->len > ops->pos ? ops->len - ops->pos : 0;

	if ((size_t)len > space)
		len = (int)space;

	memcpy(&ops->buf[ops->pos], buf, len);
	ops->pos += len;
	if (ops->pos > ops->size)
		ops->size = ops->pos;

	return len;
}

static fpos_t seekfn(void *arg, fpos_t offset, int whence)
{
	ops_t *ops = (ops_t *)arg;
	fpos_t pos;

	switch (whence) {
	case SEEK_SET:
		pos = offset;
		break;

	case SEEK_CUR:
		pos = (fpos_t)ops->pos + offset;
		break;

	case SEEK_END:
		pos = (fpos_t)ops->size + offset;
		break;

	default:
		errno = EINVAL;
		return -1;
	}

	if (pos < 0 || (size_t)pos > ops->len) {
		errno = EINVAL;
		return -1;
	}

	ops->pos = (size_t)pos;

	return pos;
}

static int closefn(void *arg)
{
	free(arg);
	return 0;
}

FILE *fmemopen(void *buf, size_t len, const char *type)
{
	ops_t *ops = malloc(sizeof(*ops));

	if (!ops)
		return NULL;

	ops->buf = buf;
	ops->len = len;
	ops->pos = 0;

	/*
	 * Match glibc: 'w' starts empty, 'r' exposes the whole buffer,
	 * and 'a' starts at the end of the existing string.
	 */
	switch (type ? type[0] : 'r') {
	case 'w':
		ops->size = 0;
		break;

	case 'a': {
		char *nul = memchr(buf, '\0', len);

		ops->size = nul ? (size_t)(nul - (char *)buf) : len;
		ops->pos = ops->size;
		break;
	}

	case 'r':
	default:
		ops->size = len;
		break;
	}

	return funopen(ops, readfn, writefn, seekfn, closefn);
}
#elif defined(HAVE_WINDOWS_H)
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>

FILE *fmemopen(void *buf, size_t len, const char *type)
{
	int fd;
	FILE *fp;
	char tp[MAX_PATH - 13];
	char fn[MAX_PATH + 1];

	if (!GetTempPathA(sizeof(tp), tp))
		return NULL;

	if (!GetTempFileNameA(tp, "confuse", 0, fn))
		return NULL;

	fd = _open(fn,
		_O_CREAT | _O_RDWR | _O_SHORT_LIVED | _O_TEMPORARY | _O_BINARY,
		_S_IREAD | _S_IWRITE);
	if (fd == -1)
		return NULL;

	fp = _fdopen(fd, "w+");
	if (!fp) {
		_close(fd);
		return NULL;
	}

	fwrite(buf, len, 1, fp);
	rewind(fp);

	return fp;
}

#else
#error Sorry, this platform currently has no fmemopen() replacement.
#endif

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
