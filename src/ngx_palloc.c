/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#include "../inc/cs.h"
#include "../inc/cs_os.h"
#include "../inc/cs_str.h"
#include "../inc/ngx_palloc.h"
#include "../inc/ngx_alloc.h"

static cs_inline void *ngx_palloc_small(
  ngx_pool_t *pool, size_t size, cs_uint_t align);
static void *ngx_palloc_block(ngx_pool_t *pool, size_t size);
static void *ngx_palloc_large(ngx_pool_t *pool, size_t size);

CS_API ngx_pool_t *ngx_create_pool(size_t size) {
  ngx_pool_t  *p;

  p = ngx_memalign(NGX_POOL_ALIGNMENT, size);
  if (p == NULL) {
    return (NULL);
  }

  p->d.last = (u_char *) p + sizeof(ngx_pool_t);
  p->d.end = (u_char *) p + size;
  p->d.next = NULL;
  p->d.failed = 0;

  size = size - sizeof(ngx_pool_t);
  p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

  p->current = p;
  p->large = NULL;

  return (p);
}

CS_API void ngx_destroy_pool(ngx_pool_t *pool) {
  ngx_pool_t          *p, *n;
  ngx_pool_large_t    *l;

  #ifdef CS_DEBUG
  int count = 0;
  #endif
  for (l = pool->large; l; l = l->next) {
    if (l->alloc) {
      ngx_free(l->alloc);
    }
    #ifdef CS_DEBUG
    count++;
    #endif
  }

  #ifdef CS_DEBUG
    printf("large count: %d\n", count);
    count = 0;
  #endif
    
  for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
    #ifdef CS_DEBUG
    count++;
    #endif
    ngx_free(p);
    if (n == NULL) {
      break;
    }
  }
  #ifdef CS_DEBUG
    printf("pool count: %d\n", count);
    count = 0;
  #endif
}

CS_API void ngx_reset_pool(ngx_pool_t *pool) {
  ngx_pool_t        *p;
  ngx_pool_large_t  *l;

  for (l = pool->large; l; l = l->next) {
    if (l->alloc) {
      ngx_free(l->alloc);
    }
  }

  for (p = pool; p; p = p->d.next) {
    p->d.last = (u_char *) p + sizeof(ngx_pool_t);
    p->d.failed = 0;
  }

  pool->current = pool;
  pool->large = NULL;
}


CS_API void *ngx_palloc(ngx_pool_t *pool, size_t size) {
#if !(NGX_DEBUG_PALLOC)
  if (size <= pool->max) {
    return ngx_palloc_small(pool, size, 1);
  }
#endif

  return ngx_palloc_large(pool, size);
}


CS_API void *ngx_pnalloc(ngx_pool_t *pool, size_t size) {
#if !(NGX_DEBUG_PALLOC)
  if (size <= pool->max) {
    return ngx_palloc_small(pool, size, 0);
  }
#endif

  return ngx_palloc_large(pool, size);
}

static cs_inline void *ngx_palloc_small(
  ngx_pool_t *pool, size_t size, cs_uint_t align) {
  u_char      *m;
  ngx_pool_t  *p;

  p = pool->current;

  do {
    m = p->d.last;

    if (align) {
      m = cs_align_ptr(m, CS_ALIGNMENT);
    }

    if ((size_t) (p->d.end - m) >= size) {
      p->d.last = m + size;

      return m;
    }

    p = p->d.next;
  } while (p);

  return (ngx_palloc_block(pool, size));
}


static void *ngx_palloc_block(ngx_pool_t *pool, size_t size) {
  u_char      *m;
  size_t       psize;
  ngx_pool_t  *p, *new;

  psize = (size_t) (pool->d.end - (u_char *) pool);

  m = ngx_memalign(NGX_POOL_ALIGNMENT, psize);
  if (m == NULL) {
    return (NULL);
  }

  new = (ngx_pool_t *) m;

  new->d.end = m + psize;
  new->d.next = NULL;
  new->d.failed = 0;

  m += sizeof(ngx_pool_data_t);
  m = cs_align_ptr(m, CS_ALIGNMENT);
  new->d.last = m + size;

  for (p = pool->current; p->d.next; p = p->d.next) {
    if (p->d.failed++ > 4) {
      pool->current = p->d.next;
    }
  }

  p->d.next = new;

  return (m);
}

static void *ngx_palloc_large(ngx_pool_t *pool, size_t size) {
  void              *p;
  cs_uint_t          n;
  ngx_pool_large_t  *large;

  p = ngx_alloc(size);
  if (p == NULL) {
    return (NULL);
  }

  n = 0;

  for (large = pool->large; large; large = large->next) {
    if (large->alloc == NULL) {
      large->alloc = p;
      return p;
    }

    if (n++ > 3) {
      break;
    }
  }

  large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
  if (large == NULL) {
    ngx_free(p);
    return (NULL);
  }

  large->alloc = p;
  large->next = pool->large;
  pool->large = large;

  return (p);
}

CS_API void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment) {
  void              *p;
  ngx_pool_large_t  *large;

  p = ngx_memalign(alignment, size);
  if (p == NULL) {
    return (NULL);
  }

  large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
  if (large == NULL) {
    ngx_free(p);
    return (NULL);
  }

  large->alloc = p;
  large->next = pool->large;
  pool->large = large;

  return (p);
}

CS_API cs_int_t ngx_pfree(ngx_pool_t *pool, void *p) {
  ngx_pool_large_t  *l;

  for (l = pool->large; l; l = l->next) {
    if (p == l->alloc) {
      ngx_free(l->alloc);
      l->alloc = NULL;

      return (CS_OK);
    }
  }

  return (CS_DECLINED);
}

CS_API void *ngx_pcalloc(ngx_pool_t *pool, size_t size) {
  void *p;

  p = ngx_palloc(pool, size);
  if (p) {
    cs_bzero(p, size);
  }

  return (p);
}
