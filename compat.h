/*
 *  Copyright (c) 2007 Nicholas Marriott <nicholas.marriott@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef COMPAT_H
#define COMPAT_H

#include <sys/types.h>
#include <sys/uio.h>

#include <limits.h>
#include <stdio.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
#include <inttypes.h>
#endif

#ifdef HAVE_QUEUE_H
#include <sys/queue.h>
#else
#include "compat/queue.h"
#endif

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#endif

#ifndef HAVE_GETDTABLECOUNT
/* getdtablecount.c */
int		 getdtablecount(void);
#endif

#ifndef HAVE_FREEZERO
/* freezero.c */
void		 freezero(void *, size_t);
#endif

#ifndef HAVE_RECALLOCARRAY
/* recallocarray.c */
void		*recallocarray(void *, size_t, size_t, size_t);
#endif

#ifdef HAVE_IMSG
#include <imsg.h>
#else
#include "compat/imsg.h"
#endif

#endif /* COMPAT_H */
