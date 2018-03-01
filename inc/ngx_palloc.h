/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
		   
#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define ngx_pagesize cs_os_pagesize()
#else
#define ngx_pagesize getpagesize()
#endif
/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16

typedef struct ngx_pool_s        ngx_pool_t;
typedef struct ngx_pool_large_s  ngx_pool_large_t;

#define NGX_MIN_POOL_SIZE        \
cs_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)), \
NGX_POOL_ALIGNMENT)

struct ngx_pool_large_s {
  ngx_pool_large_t     *next;
  void                 *alloc;
};

typedef struct {
  u_char               *last;
  u_char               *end;
  ngx_pool_t           *next;
  cs_uint_t            failed;
} ngx_pool_data_t;


struct ngx_pool_s {
  ngx_pool_data_t       d;
  size_t                max;
  ngx_pool_t           *current;
  ngx_pool_large_t     *large;
};

void       *ngx_alloc(size_t size);
void       *ngx_calloc(size_t size);

CS_API ngx_pool_t *ngx_create_pool(size_t size);
CS_API void        ngx_destroy_pool(ngx_pool_t *pool);
CS_API void        ngx_reset_pool(ngx_pool_t *pool);

CS_API void       *ngx_palloc(ngx_pool_t *pool, size_t size);
CS_API void       *ngx_pnalloc(ngx_pool_t *pool, size_t size);
CS_API void       *ngx_pcalloc(ngx_pool_t *pool, size_t size);
CS_API void       *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
CS_API cs_int_t    ngx_pfree(ngx_pool_t *pool, void *p);

#ifdef __cplusplus
}
#endif /* C++ support */
#endif /* _NGX_PALLOC_H_INCLUDED_ */