/**
 * cs_str.h
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_STR_H_INCLUDED_
#define _CS_STR_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

/* 
* converts hex char (0-9, A-Z, a-z) to decimal.
* returns 0xFF on invalid input.
	*/
#define X2C(x, c)  {                  \
	c = (u_char)(x - '0'); if (c > 9) { \
  c = (u_char)((c + '0' - 1) | 0x20); \
	c = (u_char)(c - 'a' + 11);}}

#define CS_KB  1 << 10
#define CS_MB  1 << 20

typedef struct cs_buf_s{
	u_char *buf;
	size_t used;
	size_t size;
} cs_buf_t;

#define cs_buf_str(b)     (b)->buf
#define cs_buf_used(b)    (b)->used
#define cs_buf_size(b)    (b)->size
#define cs_buf_strend(b) *((b)->buf + (b)->used) = '\0'
#define cs_buf_isempty(b) ((b)->size == 0)

#define cs_malloc         malloc
#define cs_free           free

#define CS_MALLOC(T, n)   (T *)cs_malloc(sizeof(T) * (n))

#define cs_bcpy(d, s, n)  (u_char *)memcpy(d, s, n)
#define cs_bcmp(d, s, n)  memcmp(d, s, n)
#define cs_bchr(s, c, n)  (u_char *)memchr(s, c, n)
#define cs_bset(s, v, n)  memset(s, v, n)
#define cs_bzero(s, n)    memset(s, 0, n)

CS_API size_t      cs_strerror(u_char *err, const char *fmt, ...);

CS_API size_t      cs_strlen(const u_char *s);

CS_API u_char     *cs_tolower(u_char *s);
CS_API u_char     *cs_toupper(u_char *s);

CS_API u_char     *cs_strcpy(u_char *d, const u_char *s);
CS_API u_char     *cs_strncpy(u_char *d, const u_char *s, size_t n);

CS_API u_char     *cs_strcat(u_char *d, const u_char *s);
CS_API u_char     *cs_strncat(u_char *d, const u_char *s, size_t n);

CS_API int         cs_strcmp(const u_char *s1, const u_char *s2);
CS_API int         cs_strncmp(const u_char *s1, const u_char *s2, size_t n);

CS_API int         cs_strcasecmp(u_char *s1, u_char *s2);
CS_API int         cs_strncasecmp(u_char *s1, u_char *s2, size_t n);

CS_API u_char     *cs_strchr(const u_char *s, u_char c);
CS_API u_char     *cs_strstr(const u_char *s, u_char *d);
CS_API u_char     *cs_strrchr(const u_char *s, u_char c);
CS_API size_t      cs_strchrcpy(u_char *d, const u_char *s, u_char c);

CS_API u_char     *cs_strdup(u_char *s, size_t n);

CS_API cs_buf_t   *cs_buf_init(u_char *p, size_t n);
CS_API cs_buf_t   *cs_buf_new(size_t n);
CS_API void        cs_buf_free(cs_buf_t *buf);
CS_API size_t      cs_buf_cat(cs_buf_t *d, cs_buf_t *s);
CS_API u_char     *cs_buf_bcpy(cs_buf_t *buf, u_char *s, size_t n);
CS_API u_char     *cs_buf_bcat(cs_buf_t *buf, u_char *s, size_t n);
CS_API int         cs_buf_cmp(cs_buf_t *d, cs_buf_t *s);

CS_API cs_int_t    cs_atoi(u_char *s, size_t n);
CS_API cs_uint_t   cs_atou(u_char *s, size_t n);

CS_API cs_int64_t  cs_atoi64(u_char *s, size_t n);

CS_API cs_int_t    cs_htoi(u_char *s, size_t n);
CS_API cs_uint_t   cs_htou(u_char *s, size_t n);

CS_API cs_int_t    cs_btoi(u_char *s, size_t n);
CS_API cs_int_t    cs_btou(u_char *s, size_t n);

CS_API size_t      cs_uri_escape(u_char *d, const u_char *s);
CS_API size_t      cs_uri_unescape(u_char *d, const u_char *s);

CS_API size_t      cs_json_escape(u_char *d, u_char *s, size_t n);

CS_API size_t      cs_hex_dump(u_char *d, u_char *s, size_t n);
CS_API size_t      cs_hex_undump(u_char *d, const u_char *s, size_t n);

CS_API void        cs_base64_encode(cs_buf_t *d, cs_buf_t *s);
CS_API cs_int_t    cs_base64_decode(cs_buf_t *d, cs_buf_t *s);

CS_API cs_uint_t   cs_utf8_decode(u_char **s, size_t n);
CS_API size_t      cs_utf8_length(u_char *s, size_t n);

CS_API cs_uint_t   cs_hash_djb2(const u_char *s);
CS_API cs_uint64_t cs_hash_djb2_64(const u_char *s);

CS_API u_char     *cs_md5(u_char *d, const u_char *s, size_t n);
CS_API u_char     *cs_sha1(u_char *d, const u_char *s, size_t n);

CS_API cs_uint_t   cs_crc32c(const u_char *s, size_t n);

#ifdef __cplusplus
}
#endif /* C++ support */
#endif