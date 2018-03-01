
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#include "../inc/cs.h"
#include "../inc/cs_str.h"
#include "../inc/ngx_alloc.h"

void *ngx_alloc(size_t size) {
  void  *p;

  p = malloc(size);

  return (p);
}

void *ngx_calloc(size_t size) {
  void  *p;

  p = ngx_alloc(size);

  if (p) {
    cs_bzero(p, size);
  }

  return p;
}

#if (NGX_HAVE_POSIX_MEMALIGN)
void *ngx_memalign(size_t alignment, size_t size) {
  void  *p;
  int    err;

  err = posix_memalign(&p, alignment, size);

  if (err) {
    p = NULL;
  }

  return p;
}
#elif (NGX_HAVE_MEMALIGN)
void *ngx_memalign(size_t alignment, size_t size) {
  void  *p;

  p = memalign(alignment, size);
    
  return (p);
}
#endif
