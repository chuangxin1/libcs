/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
#ifndef _NGX_ALLOC_H_INCLUDED_
#define _NGX_ALLOC_H_INCLUDED_

#include <stdlib.h>

void *ngx_alloc(size_t size);
void *ngx_calloc(size_t size);

#define ngx_free free

/*
 * Linux has memalign() or posix_memalign()
 * Solaris has memalign()
 * FreeBSD 7.0 has posix_memalign(), besides, early version's malloc()
 * aligns allocations bigger than page size at the page boundary
 */
#if (NGX_HAVE_POSIX_MEMALIGN || NGX_HAVE_MEMALIGN)
void *ngx_memalign(size_t alignment, size_t size, ngx_log_t *log);
#else
#define ngx_memalign(alignment, size)  ngx_alloc(size)
#endif

#endif /* _NGX_ALLOC_H_INCLUDED_ */
