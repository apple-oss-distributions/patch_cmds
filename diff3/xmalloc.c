/* $OpenBSD: xmalloc.c,v 1.10 2019/06/28 05:44:09 deraadt Exp $ */
/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Versions of malloc and friends that check their results, and never return
 * failure (they call fatal if they encounter an error).
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <err.h>
#ifdef __APPLE__
#include <errno.h>
#endif
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xmalloc.h"

#ifdef __APPLE__
/* From FreeBSD libc/stdlib/reallocarray.c */

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))

void *
reallocarray(void *optr, size_t nmemb, size_t size)
{

	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return (NULL);
	}
	return (realloc(optr, size * nmemb));
}
#endif

void *
xmalloc(size_t size)
{
	void *ptr;

	if (size == 0)
		errx(2, "xmalloc: zero size");
	ptr = malloc(size);
	if (ptr == NULL)
		err(2, "xmalloc: allocating %zu bytes", size);
	return ptr;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = calloc(nmemb, size);
	if (ptr == NULL)
		err(2, "xcalloc: allocating %zu * %zu bytes", nmemb, size);
	return ptr;
}

void *
xreallocarray(void *ptr, size_t nmemb, size_t size)
{
	void *new_ptr;

	new_ptr = reallocarray(ptr, nmemb, size);
	if (new_ptr == NULL)
		err(2, "xreallocarray: allocating %zu * %zu bytes",
		    nmemb, size);
	return new_ptr;
}

char *
xstrdup(const char *str)
{
	char *cp;

	if ((cp = strdup(str)) == NULL)
		err(2, "xstrdup");
	return cp;
}

int
xasprintf(char **ret, const char *fmt, ...)
{
	va_list ap;
	int i;

	va_start(ap, fmt);
	i = vasprintf(ret, fmt, ap);
	va_end(ap);

	if (i == -1)
		err(2, "xasprintf");

	return i;
}
