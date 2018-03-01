/**
 * cs_str.c
 * Copyright (c) 2007-2018 ls
 **/
#include "../inc/cs.h"
#include "../inc/cs_str.h"

static const u_char _hex_char[] = "0123456789ABCDEF";

/* everything except: ! ( ) * - . 0-9 A-Z _ a-z */
static const u_char _uri_char[] = {
	/*
	0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	*/
  /*  00 -  0F control chars */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  10 -  1F */
  /*  20 -  2F space " # $ % & ' + , / */
	1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1,  
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,  /*  30 -  3F : ; < = > ? */
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /*  40 -  4F @ */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,  /*  50 -  5F [ \ ] ^ */
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /*  60 -  6F ` */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,  /*  70 -  7F { | } ~ DEL */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  80 -  8F */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  90 -  9F */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  A0 -  AF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  B0 -  BF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  C0 -  CF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  D0 -  DF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  E0 -  EF */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /*  F0 -  FF */
};

CS_API size_t cs_strerror(u_char *err, const char *fmt, ...) {
	size_t n;
	va_list ap;
    
  if (!err) {
		return (0);
	}

  va_start(ap, fmt);
  n = (size_t)vsnprintf((char *)err, CS_KB, fmt, ap);
  va_end(ap);
	
	return (n);
}

CS_API size_t cs_strlen(const u_char *s) {
	const u_char *str;
	
	for (str = s; *str; ++str);
	
	return (str - s);
}

CS_API u_char *cs_tolower(u_char *s) {
	u_char *c;
	
	for (c = s; *c; c++) {
		if (*c >= 'A' && *c <= 'Z') {
			*c |= 32;
		}
	}
	
	return (s);
}

CS_API u_char *cs_toupper(u_char *s) {
	u_char *c;
	
	for (c = s; *c; c++) {
		if (*c >= 'a' && *c <= 'z') {
			*c &= ~32;
		}
	}
	
	return (s);
}

CS_API u_char *cs_strcpy(u_char *d, const u_char *s) {
	u_char *save = d;
	
	for (; (*d = *s) != '\0'; ++s, ++d);
	
	return (save);
}

CS_API u_char *cs_strncpy(u_char *d, const u_char *s, size_t n) {
	if (n != 0) {
		u_char *to = d;
		const u_char *from = s;
		
		do {
			if ((*to++ = *from++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*to++ = 0;
				break;
			}
		} while (--n != 0);
	}

	return (d);
}

CS_API u_char *cs_strcat(u_char *d, const u_char *s) {
	u_char *save = d;

	for (; *d; ++d);
	while ((*d++ = *s++) != '\0');

	return (save);
}

CS_API u_char *cs_strncat(u_char *d, const u_char *s, size_t n) {
	u_char *dst = d;
	const u_char *str = s;

	if (n != 0) {
		while (*dst != 0) {
			dst++;
		}
		do {
			if ((*dst = *str++) == 0) {
				break;
			}
			dst++;
		} while (--n != 0);
		*dst = 0;
	}

	return (d);
}

CS_API int cs_strcmp(const u_char *s1, const u_char *s2) {
	while (*s1 == *s2++) {
		if (*s1++ == 0) {
			return (0);
		}
	}

	return (*s1 - *--s2);
}

CS_API int cs_strncmp(const u_char *s1, const u_char *s2, size_t n) {
	if (n == 0) {
		return (0);
	}

	do {
		if (*s1 != *s2++) {
			return (*s1 - *--s2);
		}
		if (*s1++ == 0) {
			break;
		}
	} while (--n != 0);

	return (0);
}

CS_API int cs_strcasecmp(u_char *s1, u_char *s2) {
  u_char c1, c2;
     
	for ( ; ; ) {
    c1 = *s1++;
    c2 = *s2++;
		
    c1 = (u_char)((c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1);
    c2 = (u_char)((c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2);
		
    if (c1 == c2) {			
      if (c1) {
        continue;
      }

      return (0);
    }

    return (c1 - c2);
  }
}

CS_API int cs_strncasecmp(u_char *s1, u_char *s2, size_t n) {
	u_char c1, c2;
	
	while (n) {
    c1 = *s1++;
    c2 = *s2++;
		
    c1 = (u_char)((c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1);
    c2 = (u_char)((c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2);
		
    if (c1 == c2) {			
      if (c1) {
        n--;
        continue;
      }

      return (0);
    }		
    return (c1 - c2);
  }
	
  return (0);
}

CS_API u_char *cs_strchr(const u_char *s, u_char c) {
	const u_char *p = s;
	
	for ( ; ; ) {
		if (*p == c) {
			return ((u_char *)p);
		}
		if (!*p) {
			return (NULL);
		}
		p++;
	}
}

CS_API u_char *cs_strstr(const u_char *s, u_char *d) {
	register u_char *cp = (u_char *)s;
	register u_char *s1, *s2;
	
	if (!*d) {
		return ((u_char *)s);
	}
	
	while (*cp) {
		s1 = cp;
		s2 = (u_char *)d;
		
		while (*s1 && *s2 && !(*s1 - *s2)) {
			s1++, s2++;
		}
		if (!*s2) {
			return (cp);
		}
		cp++;
	}
	
  return (NULL);
}

CS_API u_char *cs_strrchr(const u_char *s, u_char c) {
	u_char *start = (u_char *)s;
	
	/* find end of string */
	while (*s++);
	/* search towards front */
	while (--s != start && *s != c);
	
	if (*s == c) { /* char found ? */
		return ((u_char *)s);
	}
	
	return (NULL);
}

CS_API size_t cs_strchrcpy(u_char *d, const u_char *s, u_char c) {
	register u_char *cp = (u_char *)s;

	while (*cp) {
		if (*cp == c) {
			break;
		}
		if (!*cp) {
			return (0);
		}
		*d++ = *cp;
		cp++;
	}

	return (cp - s);
}

CS_API u_char *cs_strdup(u_char *s, size_t n) {
	u_char *cp;
	
	cp = (u_char *)malloc(n + sizeof(u_char));
	if (cp) {
		cs_bcpy(cp, s, n);
		*(cp + n) = '\0';

		return (cp);
	}

	return (NULL);
}

CS_API cs_buf_t *cs_buf_init(u_char *p, size_t n) {
	cs_buf_t *buf = (cs_buf_t *)p;

	buf->buf = p + sizeof(cs_buf_t);
	buf->used = 0;
	buf->size = n - sizeof(cs_buf_t);

	return (buf);
}

CS_API cs_buf_t *cs_buf_new(size_t n) {
	cs_buf_t *buf = NULL;

	u_char *p = (u_char *)cs_malloc((sizeof(cs_buf_t) + n) * sizeof(u_char));
	if (p) {
		buf = (cs_buf_t *)p;
		buf->buf = p + sizeof(cs_buf_t);
		buf->size = n;
		buf->used = 0;
	}

	return (buf);
}

CS_API void cs_buf_free(cs_buf_t *buf) {
	if (buf) {
		cs_free(buf);
	}
}

CS_API size_t cs_buf_cat(cs_buf_t *d, cs_buf_t *s) {
	size_t n = d->used;

	if ((d->used + s->used) > d->size) {
		n = s->size - s->used;
	}

	cs_bcpy(d->buf + d->used, s->buf, n);
	d->used += n;

	return (n);
}

CS_API u_char *cs_buf_bcpy(cs_buf_t *buf, u_char *s, size_t n) {
	u_char *p = buf->buf;

	cs_bcpy(buf->buf + buf->used, s, n);
	buf->used = n;

	return (p);
}

CS_API u_char *cs_buf_bcat(cs_buf_t *buf, u_char *s, size_t n) {
	u_char *p = buf->buf;
	size_t len = n;

	if ((buf->used + n) > buf->size) {
		len = buf->size - buf->used;
	}

	cs_bcpy(buf->buf + buf->used, s, len);
	buf->used += len;

	return (p);
}

CS_API int cs_buf_cmp(cs_buf_t *d, cs_buf_t *s) {
	if (d->used == s->used) {
		return cs_bcmp(d->buf, s->buf, d->used);
	}
	return (int)(d->used - s->used);
}

CS_API cs_int_t	cs_atoi(u_char *s, size_t n) {
	cs_int_t  value, neg, c;
	
  if (n == 0) {
    return (0);
  }
	
	c = *s;
	if (c == '-') {
		neg = 1;
		s++;
		n--;
	} else {
		neg = 0;
	}
	
  for (value = 0; n--; s++) {
    if (*s < '0' || *s > '9') {
      return (0);
  	}

    value = value * 10 + (*s - '0');
  }
	
	if (neg) {
		value = value * -1;
	}
	
	return (value);
}

CS_API cs_int64_t cs_atoi64(u_char *s, size_t n) {
	cs_int64_t  value;
	cs_int_t neg, c;
	
  if (n == 0) {
  	return (0);
  }

	c = *s;
	if (c == '-') {
		neg = 1;
		s++;
		n--;
	} else {
		neg = 0;
	}

  for (value = 0; n--; s++) {
    if (*s < '0' || *s > '9') {
      return (0);
    }
		
    value = value * 10 + (*s - '0');
  }

	if (neg) {
		value = value * -1;
	}
	
  return (value);
}

CS_API cs_uint_t cs_atou(u_char *s, size_t n) {
	cs_uint_t value;
	
  if (n == 0) {
    return (0);
  }
	
  for (value = 0; n--; s++) {
    if (*s < '0' || *s > '9') {
      return (0);
    }
		
    value = value * 10 + (*s - '0');
  }
	
  return (value);
}

CS_API cs_int_t	cs_htoi(u_char *s, size_t n) {
	u_char c, ch;
	cs_int_t value;

	for (value = 0; n--; s++){
		ch = *s;
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');
      continue;
    }
		
    c = (u_char) (ch | 0x20);
		
    if (c >= 'a' && c <= 'f') {
      value = value * 16 + (c - 'a' + 10);
      continue;
    }
	}

	return (value);
}

CS_API cs_uint_t cs_htou(u_char *s, size_t n) {
	u_char c, ch;
	cs_uint_t value;
	
	for (value = 0; n--; s++){
		ch = *s;
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');
      continue;
    }
		
    c = (u_char) (ch | 0x20);
		
    if (c >= 'a' && c <= 'f') {
      value = value * 16 + (c - 'a' + 10);
      continue;
    }
	}
	
	return (value);
}

CS_API cs_int_t cs_btoi(u_char *s, size_t n) {
	u_char c, ch;
	cs_int_t value;	
	
	for (value = 0; n--; s++) {
		ch = _hex_char[(*s) >> 4];
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');            
    } else {
			c = (u_char) (ch | 0x20);
			
			if (c >= 'a' && c <= 'f') {
				value = value * 16 + (c - 'a' + 10);				
			}
		}		
        
		ch = _hex_char[(*s) & 0x0f];
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');            
    } else {
			c = (u_char) (ch | 0x20);
			
			if (c >= 'a' && c <= 'f') {
				value = value * 16 + (c - 'a' + 10);				
			}
		}
	}
	
	return (value);
}

CS_API cs_int_t cs_btou(u_char *s, size_t n) {
	u_char c, ch;
	cs_uint_t value;	
	
	for (value = 0; n--; s++) {
		ch = _hex_char[(*s) >> 4];
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');            
    } else {
			c = (u_char) (ch | 0x20);
			
			if (c >= 'a' && c <= 'f') {
				value = value * 16 + (c - 'a' + 10);				
			}
		}		
        
		ch = _hex_char[(*s) & 0x0f];
		if (ch >= '0' && ch <= '9') {
      value = value * 16 + (ch - '0');            
    } else {
			c = (u_char) (ch | 0x20);
			
			if (c >= 'a' && c <= 'f') {
				value = value * 16 + (c - 'a' + 10);				
			}
		}
	}
	
	return (value);
}

CS_API size_t cs_uri_escape(u_char *d, const u_char *s) {
	u_char	*cp = d;
	size_t n;

	if (d == NULL) {
		n = 0;
		while (*s) {
			if (_uri_char[*s]) {
				n++;
			}
			s++;
		}

		return (n);
	}
	
	while (*s) {
		if (_uri_char[*s]) {
			*d++ = '%';
			*d++ = _hex_char[(*s) >> 4];
			*d++ = _hex_char[(*s) & 0x0f];
			s++;		
		} else {
			*d++ = *s++;
		}
	}
	
	return (d - cp);
}

CS_API size_t cs_uri_unescape(u_char *d, const u_char *s) {
	register unsigned char high, low;
	u_char *cp = d;

	while ((*s) != '\0') {
		if (*s == '%') {
			X2C(*(s + 1), high);
			if (high < 0x10) {
				X2C(*(s + 2), low);
				if (low < 0x10) {
					high = (u_char)((high << 4) | low);
					*d = high;
					s += 2;
				}
			}
		} else if (*s == '+') {
			*d = ' ';
		} else {
			*d = *s;
		}		
		d++;
		s++;
	}
	
	return (d - cp);
}

/* from ngx_string.c */
CS_API size_t cs_json_escape(u_char *d, u_char *s, size_t n) {
	u_char ch, *cp = d;
  size_t len;

  if (d == NULL) {
    len = 0;

    while (n) {
      ch = *s++;

      if (ch == '\\' || ch == '"') {
        len++;
      } else if (ch <= 0x1f) {
        len += sizeof("\\u001F") - 2;
      }

      n--;
    }

    return (len);
  }

  while (n) {
    ch = *s++;

    if (ch > 0x1f) {
      if (ch == '\\' || ch == '"') {
        *d++ = '\\';
      }

      *d++ = ch;
    } else {
      *d++ = '\\'; *d++ = 'u'; *d++ = '0'; *d++ = '0';
      *d++ = '0' + (ch >> 4);

      ch &= 0xf;

      *d++ = (ch < 10) ? ('0' + ch) : ('A' + ch - 10);
    }

    n--;
  }

  return (d - cp);
}

CS_API size_t cs_hex_dump(u_char *d, u_char *s, size_t n) { 
	u_char *cp = d;
	
  while (n--) {
    *d++ = _hex_char[*s >> 4];
    *d++ = _hex_char[*s++ & 0xf];
  }
	
  return (d - cp);
}

CS_API size_t cs_hex_undump(u_char *d, const u_char *s, size_t n) {
	size_t i = 0;
	register u_char ch, digit;
	u_char *cp = d;
	
	for (i = 0; i < n; i++) {
		X2C(*(s + i) ,ch);
		ch = ch << 4;
		i++;
		X2C(*(s + i), digit);
		ch += digit;
		*d++ = ch;
	}
	
	return (d - cp);
}

/* from ngx_string.c */
static void _cs_base64_encode_internal(
	cs_buf_t *dst, cs_buf_t *src, const u_char *basis, cs_uint_t padding) {
	u_char *d, *s;
  size_t len;

  len = src->used;
  s = src->buf;
  d = dst->buf;

  while (len > 2) {
    *d++ = basis[(s[0] >> 2) & 0x3f];
    *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
    *d++ = basis[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
    *d++ = basis[s[2] & 0x3f];

    s += 3;
    len -= 3;
  }

  if (len) {
    *d++ = basis[(s[0] >> 2) & 0x3f];

    if (len == 1) {
      *d++ = basis[(s[0] & 3) << 4];
      if (padding) {
        *d++ = '=';
      }
    } else {
      *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
      *d++ = basis[(s[1] & 0x0f) << 2];
    }

    if (padding) {
      *d++ = '=';
    }
  }

  dst->used = d - dst->buf;
}

/* from ngx_string.c */
CS_API void cs_base64_encode(cs_buf_t *d, cs_buf_t *s) {
  static u_char   basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  _cs_base64_encode_internal(d, s, basis64, 1);
}

/* from ngx_string.c */
static cs_int_t _cs_base64_decode_internal(
	cs_buf_t *dst, cs_buf_t *src, const u_char *basis) {
  size_t len;
  u_char *d, *s;

  for (len = 0; len < src->used; len++) {
    if (src->buf[len] == '=') {
      break;
    }

    if (basis[src->buf[len]] == 77) {
      return (CS_ERR);
    }
  }

  if (len % 4 == 1) {
    return (CS_ERR);
  }

  s = src->buf;
  d = dst->buf;

  while (len > 3) {
    *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    *d++ = (u_char) (basis[s[2]] << 6 | basis[s[3]]);

    s += 4;
    len -= 4;
  }

  if (len > 1) {
    *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
  }

  if (len > 2) {
    *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
  }

  dst->used = d - dst->buf;

  return (CS_OK);
}

/* from ngx_string.c */
CS_API cs_int_t cs_base64_decode(cs_buf_t *d, cs_buf_t *s) {
  static u_char   basis64[] = {
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
    77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
    77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
  };

  return _cs_base64_decode_internal(d, s, basis64);
}

/* from ngx_string.c */
CS_API cs_uint_t cs_utf8_decode(u_char **s, size_t n) {
	size_t    len;
  cs_uint_t  u, i, valid;
	
  u = **s;
	
  if(u > 0xf0){
    u &= 0x07;
    valid = 0xffff;
    len = 3;		
  } else if (u > 0xe0) {		
    u &= 0x0f;
    valid = 0x7ff;
    len = 2;		
  } else if (u > 0xc0) {		
    u &= 0x1f;
    valid = 0x7f;
    len = 1;
  } else{
    (*s)++;

    return (0xffffffff);
  }
	
  if (n - 1 < len) {
    return (0xfffffffe);
  }
	
  (*s)++;
	
  while (len) {
    i = *(*s)++;
		
    if (i < 0x80) {
      return (0xffffffff);
  	}
		
    u = (u << 6) | (i & 0x3f);		
    len--;
  }
	
  if (u > valid) {
    return (u);
  }
	
  return (0xffffffff);
}

/* from ngx_string.c */
CS_API size_t cs_utf8_length(u_char *s, size_t n) {
	u_char  c, *last;
  size_t  len;
	
  last = s + n;
  for (len = 0; s < last; len++) {		
    c = *s;
    if (c < 0x80) {
      s++;
      continue;
    }
		
    if (cs_utf8_decode(&s, n) > 0x10ffff) {
      /* invalid UTF-8 */
      return n;
    }
  }

  return (len);
}

/*
djb2
	this algorithm (k=33) was first reported by dan bernstein many years ago 
in comp.lang.c.
another version of this algorithm (now favored by bernstein) uses xor:
	hash(i) = hash(i - 1) * 33 ^ str[i];
	the magic of number 33 (why it works better than many other constants, 
prime or not)
has never been adequately explained.
*/
CS_API cs_uint_t cs_hash_djb2(const u_char *s) {
	const u_char *cp = s;
	cs_uint_t hash = 5381;

	while (*cp) {
		hash = ((hash << 5) + hash) + *cp; /* hash = hash * 33 + c */
		cp++;
	}

	return (hash);
}

CS_API cs_uint64_t cs_hash_djb2_64(const u_char *s) {
	const u_char *cp = s;
	cs_uint64_t hash = 5381;
	
	while (*cp) {
		hash = ((hash << 5) + hash) + *cp; /* hash = hash * 33 + c */
		cp++;
	}
	
	return (hash);
}